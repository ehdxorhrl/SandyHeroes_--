﻿#include "stdafx.h"
#include "GameFramework.h"
#include "Packet.h"
#include "Timer.h"
#include "Object.h"
#include "Shader.h"
#include "AITestScene.h"
#include "FrameResourceManager.h"
#include "DescriptorManager.h"
#include "Mesh.h"
#include "InputManager.h"
#include "InputControllerComponent.h"
#include "ModelInfo.h"
#include "Material.h"
#include "AnimationSet.h"
#include "BaseScene.h"

GameFramework* GameFramework::kGameFramework = nullptr;


GameFramework::GameFramework() 
{
	assert(kGameFramework == nullptr);
	kGameFramework = this;
}

GameFramework::~GameFramework()
{
    if (d3d_device_)
    {
        FlushCommandQueue();
        if (client_full_screen_state_)
        {
            ChangeWindowMode();
        }
    }
    closesocket(socket_);
    WSACleanup();
}

void GameFramework::Initialize(HINSTANCE hinstance, HWND hwnd)
{
    app_instance_ = hinstance;
    main_wnd_ = hwnd;

    InitDirect3D();

    d3d_command_list_->Reset(d3d_command_allocator_.Get(), nullptr);

    BuildRootSignature();

    frame_resource_manager_ = std::make_unique<FrameResourceManager>();
    descriptor_manager_ = std::make_unique<DescriptorManager>();
    input_manager_ = std::make_unique<InputManager>();

    //씬 생성 및 초기화
    scene_ = std::make_unique<BaseScene>();
    scene_->Initialize(d3d_device_.Get(), d3d_command_list_.Get(), d3d_root_signature_.Get(), 
        this);

    OnResize();

    d3d_command_list_->Close();
    ID3D12CommandList* cmd_list[] = { d3d_command_list_.Get() };
    d3d_command_queue_->ExecuteCommandLists(1, cmd_list);

    FlushCommandQueue();

    scene_->ReleaseMeshUploadBuffer();

    client_timer_.reset(new Timer);
    client_timer_->Reset();
}

void GameFramework::InitDirect3D()
{
    UINT dxgi_farctory_flags = 0;

#if defined(DEBUG) | defined(_DEBUG)
    // D3D12 디버그 레이어를 활성화 합니다.
    {
        ComPtr<ID3D12Debug> debeg_controller;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debeg_controller))))
        {
            debeg_controller->EnableDebugLayer();

            dxgi_farctory_flags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    CreateDXGIFactory2(dxgi_farctory_flags, IID_PPV_ARGS(&dxgi_factory_));

    // 하드웨어 디바이스의 생성을 시도합니다.
    HRESULT hardwareResult = D3D12CreateDevice(
        nullptr, // 기본 어뎁터 사용합니다.
        D3D_FEATURE_LEVEL_12_0,
        IID_PPV_ARGS(&d3d_device_));

    // 실패할 경우 WRAP 디바이스를 사용합니다.
    if (FAILED(hardwareResult))
    {
        ComPtr<IDXGIAdapter> pWrapAdapter;
        dxgi_factory_->EnumWarpAdapter(IID_PPV_ARGS(&pWrapAdapter));

        D3D12CreateDevice(
            pWrapAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&d3d_device_));
    }

    if (!d3d_device_)
    {
        MessageBox(NULL, L"Direct3D 12 Device Cannot be Created.", L"Error", MB_OK);
        ::PostQuitMessage(0);
        return;
    }

    //펜스 생성
    d3d_device_->CreateFence(current_fence_value_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&d3d_fence_));

    rtv_descriptor_size_ = d3d_device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    dsv_descriptor_size_ = d3d_device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    cbv_srv_uav_descriptor_size_ = d3d_device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    DescriptorManager::kCbvSrvUavDescriptorSize = cbv_srv_uav_descriptor_size_;

    //4x msaa 검사
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS quality_levels = {};
    quality_levels.Format = back_buffer_format_;
    quality_levels.SampleCount = 4;
    quality_levels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    quality_levels.NumQualityLevels = 0;
    d3d_device_->CheckFeatureSupport(
        D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
        &quality_levels,
        sizeof(quality_levels));

    msaa_quality_ = quality_levels.NumQualityLevels;
    assert(msaa_quality_ > 0 && "Unexpected MSAA quality level.");

    CreateCommandObject();
    CreateSwapChain();
    CreateRtvAndDsvDescriptorHeaps();

}

void GameFramework::CreateCommandObject()
{
    D3D12_COMMAND_QUEUE_DESC queue_desc = {};
    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    HRESULT hresult = d3d_device_->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&d3d_command_queue_));

    d3d_device_->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT, 
        IID_PPV_ARGS(&d3d_command_allocator_));

    d3d_device_->CreateCommandList(
        0, 
        D3D12_COMMAND_LIST_TYPE_DIRECT, 
        d3d_command_allocator_.Get(),
        nullptr, 
        IID_PPV_ARGS(&d3d_command_list_));

    // 렌더링 하기에 앞서서 커맨드 리스트를 리셋하고 커맨드를 기록하는데,
    // 리셋을 하기 위해선 커맨드 리스트가 닫혀있는 상태여야 합니다.
    d3d_command_list_->Close();
}

void GameFramework::CreateSwapChain()
{
    DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
    swap_chain_desc.BufferDesc.Width = client_width_;
    swap_chain_desc.BufferDesc.Height = client_height_;
    swap_chain_desc.BufferDesc.RefreshRate.Numerator = client_refresh_rate_;
    swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
    swap_chain_desc.BufferDesc.Format = back_buffer_format_;
    swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swap_chain_desc.SampleDesc.Count = msaa_state_ ? 4 : 1;
    swap_chain_desc.SampleDesc.Quality = msaa_state_ ? (msaa_quality_ - 1) : 0;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.BufferCount = kSwapChainBufferCount;
    swap_chain_desc.OutputWindow = main_wnd_;
    swap_chain_desc.Windowed = true;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    HRESULT hresult = dxgi_factory_->CreateSwapChain(
        d3d_command_queue_.Get(),
        &swap_chain_desc,
        (IDXGISwapChain**)dxgi_swap_chain_.GetAddressOf());

    dxgi_factory_->MakeWindowAssociation(main_wnd_, DXGI_MWA_NO_ALT_ENTER);

    int i = 0;
    ++i;
}

void GameFramework::CreateRtvAndDsvDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
    rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtv_heap_desc.NumDescriptors = kSwapChainBufferCount;
    rtv_heap_desc.NodeMask = 0;
    rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    d3d_device_->CreateDescriptorHeap(&rtv_heap_desc, 
        IID_PPV_ARGS(d3d_rtv_heap_.GetAddressOf()));

    D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_desc = {};
    dsv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsv_heap_desc.NumDescriptors = kDepthStencilBufferCount;
    dsv_heap_desc.NodeMask = 0;
    dsv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    d3d_device_->CreateDescriptorHeap(&dsv_heap_desc,
        IID_PPV_ARGS(d3d_dsv_heap_.GetAddressOf()));

}

void GameFramework::BuildRootSignature()
{
    CD3DX12_DESCRIPTOR_RANGE descriptor_range[7];
    descriptor_range[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); //albedo
    descriptor_range[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); //spec gloss
    descriptor_range[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2); //metal gloss
    descriptor_range[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3); //emission
    descriptor_range[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4); //normal
    descriptor_range[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5); //cube
    descriptor_range[6].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 6); //shadow

    constexpr int root_parameter_size{ 14 };
    CD3DX12_ROOT_PARAMETER root_parameter[root_parameter_size];

    //25.02.23 수정
    //기존 루트 디스크립터 테이블에서 루트 CBV사용으로 변경
    root_parameter[0].InitAsConstantBufferView(0); // world matrix
    root_parameter[1].InitAsConstantBufferView(1); // bone transform
    root_parameter[2].InitAsConstantBufferView(2); // bone offset (default buffer)
    root_parameter[3].InitAsConstantBufferView(3); // ui
    root_parameter[4].InitAsConstantBufferView(4); // render pass
    root_parameter[5].InitAsConstantBufferView(5); // material
    root_parameter[6].InitAsDescriptorTable(1, &descriptor_range[0], D3D12_SHADER_VISIBILITY_PIXEL);
    root_parameter[7].InitAsDescriptorTable(1, &descriptor_range[1], D3D12_SHADER_VISIBILITY_PIXEL);
    root_parameter[8].InitAsDescriptorTable(1, &descriptor_range[2], D3D12_SHADER_VISIBILITY_PIXEL);
    root_parameter[9].InitAsDescriptorTable(1, &descriptor_range[3], D3D12_SHADER_VISIBILITY_PIXEL);
    root_parameter[10].InitAsDescriptorTable(1, &descriptor_range[4], D3D12_SHADER_VISIBILITY_PIXEL);
    root_parameter[11].InitAsDescriptorTable(1, &descriptor_range[5], D3D12_SHADER_VISIBILITY_PIXEL);
    root_parameter[12].InitAsDescriptorTable(1, &descriptor_range[6], D3D12_SHADER_VISIBILITY_PIXEL);
    root_parameter[13].InitAsConstantBufferView(6); // shadow pass

    
    CD3DX12_STATIC_SAMPLER_DESC aniso_warp{ 0 };    //���� ���͸� warp ��� ���÷�
    CD3DX12_STATIC_SAMPLER_DESC linear_warp{ 1, D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT };
    CD3DX12_STATIC_SAMPLER_DESC shadow_sampler
    { 2,
        D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
        D3D12_TEXTURE_ADDRESS_MODE_BORDER,
        D3D12_TEXTURE_ADDRESS_MODE_BORDER,
        D3D12_TEXTURE_ADDRESS_MODE_BORDER,
        0.0f,
        16,
        D3D12_COMPARISON_FUNC_LESS_EQUAL,
        D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK
    };

    constexpr int static_sampler_size = 3;
    D3D12_STATIC_SAMPLER_DESC static_sampler[static_sampler_size]{ aniso_warp , linear_warp, shadow_sampler };

    CD3DX12_ROOT_SIGNATURE_DESC root_sig_desc(root_parameter_size, root_parameter, static_sampler_size, static_sampler,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> serialized_root_sig = nullptr;
    ComPtr<ID3DBlob> error_blob = nullptr;
    D3D12SerializeRootSignature(&root_sig_desc, D3D_ROOT_SIGNATURE_VERSION_1,
        serialized_root_sig.GetAddressOf(), error_blob.GetAddressOf());

    if (error_blob)
    {
        OutputDebugStringA((char*)error_blob->GetBufferPointer());
    }

    d3d_device_->CreateRootSignature(
        0,
        serialized_root_sig->GetBufferPointer(),
        serialized_root_sig->GetBufferSize(),
        IID_PPV_ARGS(d3d_root_signature_.GetAddressOf()));

}


void GameFramework::OnResize()
{
    FlushCommandQueue();

    d3d_command_list_->Reset(d3d_command_allocator_.Get(), nullptr);

    //�ĸ���۸� �ٽ� ����� ���� ���� ���� ����
    for (ComPtr<ID3D12Resource>& swap_chain_buffer : d3d_swap_chain_buffers_)
        swap_chain_buffer.Reset();

    d3d_depth_stencil_buffer_.Reset();
    d3d_shadow_depth_buffer_.Reset();

    //�ĸ���� ũ�� ����
    dxgi_swap_chain_->ResizeBuffers(
        kSwapChainBufferCount,
        client_width_, client_height_,
        back_buffer_format_,
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

    current_back_buffer_ = 0;

    //Rtv ����
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_heap_handle(
        d3d_rtv_heap_->GetCPUDescriptorHandleForHeapStart());
    for (int i = 0; i < kSwapChainBufferCount; ++i)
    {
        //���� ��������
        dxgi_swap_chain_->GetBuffer(i,
            IID_PPV_ARGS(d3d_swap_chain_buffers_[i].GetAddressOf()));
        //�� ����
        d3d_device_->CreateRenderTargetView(
            d3d_swap_chain_buffers_[i].Get(), nullptr, rtv_heap_handle);

        //�� ������ �̵�
        rtv_heap_handle.Offset(1, rtv_descriptor_size_);
    }


    //Depth-Stencil ���� ����
    D3D12_RESOURCE_DESC ds_buffer_desc = {};
    ds_buffer_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    ds_buffer_desc.Alignment = 0;
    ds_buffer_desc.Width = client_width_;
    ds_buffer_desc.Height = client_height_;
    ds_buffer_desc.DepthOrArraySize = 1;
    ds_buffer_desc.MipLevels = 1;
    ds_buffer_desc.Format = depth_stencil_buffer_format_;
    ds_buffer_desc.SampleDesc.Count = msaa_state_ ? 4 : 1; //msaa�� ����ϸ� 4 �ƴϸ� 1
    ds_buffer_desc.SampleDesc.Quality = msaa_state_ ? (msaa_quality_ - 1) : 0;
    ds_buffer_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    ds_buffer_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE ds_clear_value = {};
    ds_clear_value.Format = depth_stencil_buffer_format_;
    ds_clear_value.DepthStencil.Depth = 1.f;
    ds_clear_value.DepthStencil.Stencil = 0;

    d3d_device_->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &ds_buffer_desc,
        D3D12_RESOURCE_STATE_COMMON,
        &ds_clear_value,
        IID_PPV_ARGS(d3d_depth_stencil_buffer_.GetAddressOf()));

    //Dsv ����
    // ���� ds ���۸� ������ �� ������ �ٲ�ų� ���ۿ� ���� 
    // ������ desc�� ������� �ʴٸ� �Ʒ� �ּ��� ����ؼ� dsv�� �����ؾ���
    /*D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Format = mDepthStencilFormat;
    dsvDesc.Texture2D.MipSlice = 0;*/
    d3d_device_->CreateDepthStencilView(
        d3d_depth_stencil_buffer_.Get(),
        nullptr,
        DepthStencilView());

    //�������۸� ��� ���� ���·� ����
    d3d_command_list_->ResourceBarrier(1,
        &CD3DX12_RESOURCE_BARRIER::Transition(
            d3d_depth_stencil_buffer_.Get(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_DEPTH_WRITE));

    //Create ShadowDepth Buffer
    {
        D3D12_RESOURCE_DESC shadowDesc = {};
        shadowDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        shadowDesc.Alignment = 0;
        shadowDesc.Width = SHADOW_MAP_SIZE;
        shadowDesc.Height = SHADOW_MAP_SIZE;
        shadowDesc.DepthOrArraySize = 1;
        shadowDesc.MipLevels = 1;
        shadowDesc.Format = depth_stencil_buffer_format_;
        shadowDesc.SampleDesc.Count = msaa_state_ ? 4 : 1;
        shadowDesc.SampleDesc.Quality = msaa_state_ ? (msaa_quality_ - 1) : 0;
        shadowDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        shadowDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        D3D12_CLEAR_VALUE shadowClear = {};
        shadowClear.Format = depth_stencil_buffer_format_;
        shadowClear.DepthStencil.Depth = 1.f;
        shadowClear.DepthStencil.Stencil = 0;

        d3d_device_->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &shadowDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            &shadowClear,
            IID_PPV_ARGS(d3d_shadow_depth_buffer_.GetAddressOf()));

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
        srvDesc.Texture2D.PlaneSlice = 0;

        auto srvHandle = descriptor_manager_->GetCpuHandle(descriptor_manager_->texture_count() - 1);

        d3d_device_->CreateShaderResourceView(d3d_shadow_depth_buffer_.Get(), &srvDesc, srvHandle);

        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
        dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Format = depth_stencil_buffer_format_;
        dsvDesc.Texture2D.MipSlice = 0;

        auto dsvHandle = DepthStencilView();
        dsvHandle.ptr += dsv_descriptor_size_;

        d3d_device_->CreateDepthStencilView(d3d_shadow_depth_buffer_.Get(), &dsvDesc, dsvHandle);
    }

    //���� ����
    d3d_command_list_->Close();
    ID3D12CommandList* command_list[] = { d3d_command_list_.Get() };
    d3d_command_queue_->ExecuteCommandLists(_countof(command_list), command_list);

    FlushCommandQueue();

    //����Ʈ ������Ʈ
    client_viewport_.TopLeftX = 0;
    client_viewport_.TopLeftY = 0;
    client_viewport_.Width = client_width_;
    client_viewport_.Height = client_height_;
    client_viewport_.MaxDepth = 1.f;
    client_viewport_.MinDepth = 0;

    scissor_rect_ = { 0, 0, client_width_, client_height_ };

    {
        shadow_viewport_.TopLeftX = 0;
        shadow_viewport_.TopLeftY = 0;
        shadow_viewport_.Width = SHADOW_MAP_SIZE;
        shadow_viewport_.Height = SHADOW_MAP_SIZE;
        shadow_viewport_.MaxDepth = 1.f;
        shadow_viewport_.MinDepth = 0;

        shadow_scissor_rect_ = { 0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE };
    }
}

void GameFramework::ChangeWindowMode()
{
    client_full_screen_state_ = !client_full_screen_state_;
    dxgi_swap_chain_->SetFullscreenState(client_full_screen_state_, nullptr);

    DXGI_MODE_DESC dxgiTargetParameters;
    dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    dxgiTargetParameters.Width = client_width_;
    dxgiTargetParameters.Height = client_height_;
    dxgiTargetParameters.RefreshRate.Numerator = 60;
    dxgiTargetParameters.RefreshRate.Denominator = 1;
    dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    dxgi_swap_chain_->ResizeTarget(&dxgiTargetParameters);

    OnResize();
}

void GameFramework::ProcessInput()
{
    while (!input_manager_->IsEmpty())
    {
        InputMessage message = input_manager_->DeQueueInputMessage(client_timer_->PlayTime());
        ProcessInput(message.id, message.w_param, message.l_param, message.time);
    }
}

void GameFramework::ProcessInput(UINT id, WPARAM w_param, LPARAM l_param, float time)
{
    switch (id)
    {
    case WM_KEYDOWN:
        if (w_param == VK_ESCAPE)
        {
            PostQuitMessage(0);
        }
        if (w_param == 'P')
        {
            FlushCommandQueue();

            d3d_command_list_->Reset(d3d_command_allocator_.Get(), nullptr);

            scene_ = std::make_unique<AITestScene>();
            scene_->Initialize(d3d_device_.Get(), d3d_command_list_.Get(), d3d_root_signature_.Get(),
                this);

            d3d_command_list_->Close();
            ID3D12CommandList* cmd_list[] = { d3d_command_list_.Get() };
            d3d_command_queue_->ExecuteCommandLists(1, cmd_list);

            FlushCommandQueue();

            scene_->ReleaseMeshUploadBuffer();

        }
        if (w_param == VK_F9)
        {
            ChangeWindowMode();
        }
        break;
    default:
        break;
    }
}

void GameFramework::FrameAdvance()
{
    client_timer_->Tick();

    //인풋 처리
    ProcessInput();

    //충돌처리
    //scene_->CheckObjectByObjectCollisions();

    //업데이트
    scene_->Update(client_timer_->ElapsedTime());
    scene_->UpdateObjectWorldMatrix();

    //렌더
    auto& command_allocator = frame_resource_manager_->curr_frame_resource()->d3d_allocator;

    command_allocator->Reset();

    d3d_command_list_->Reset(command_allocator.Get(), nullptr);
    d3d_command_list_->SetGraphicsRootSignature(d3d_root_signature_.Get());


    d3d_command_list_->RSSetViewports(1, &shadow_viewport_);
    d3d_command_list_->RSSetScissorRects(1, &shadow_scissor_rect_);

    d3d_command_list_->ResourceBarrier(1,
        &CD3DX12_RESOURCE_BARRIER::Transition(
            d3d_shadow_depth_buffer_.Get(),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            D3D12_RESOURCE_STATE_DEPTH_WRITE));

    auto dsvHandle = DepthStencilView();
    dsvHandle.ptr += dsv_descriptor_size_;

    d3d_command_list_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);
    d3d_command_list_->OMSetRenderTargets(0, nullptr, false, &dsvHandle);

    scene_->ShadowRender(d3d_command_list_.Get());

    d3d_command_list_->ResourceBarrier(1,
        &CD3DX12_RESOURCE_BARRIER::Transition(
            d3d_shadow_depth_buffer_.Get(),
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            D3D12_RESOURCE_STATE_GENERIC_READ));

    ID3D12DescriptorHeap* descriptor_heaps[] = { descriptor_manager_->GetDescriptorHeap() };
    d3d_command_list_->SetDescriptorHeaps(_countof(descriptor_heaps), descriptor_heaps);

    auto shadowHandle = descriptor_manager_->GetGpuHandle(descriptor_manager_->texture_count() - 1);
    d3d_command_list_->SetGraphicsRootDescriptorTable((int)RootParameterIndex::kShadowMap, shadowHandle);

    d3d_command_list_->ResourceBarrier(1,
        &CD3DX12_RESOURCE_BARRIER::Transition(
            d3d_swap_chain_buffers_[current_back_buffer_].Get(),
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET));

    d3d_command_list_->RSSetViewports(1, &client_viewport_);
    d3d_command_list_->RSSetScissorRects(1, &scissor_rect_);

    d3d_command_list_->ClearRenderTargetView(
        CurrentBackBufferView(), DirectX::Colors::Purple, 0, nullptr);
    d3d_command_list_->ClearDepthStencilView(
        DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);

    const UINT kNumRenderTargetDescriptors = 1; // 렌더대상의 개수
    d3d_command_list_->OMSetRenderTargets(
        kNumRenderTargetDescriptors,
        &CurrentBackBufferView(),
        true, &DepthStencilView());



    scene_->Render(d3d_command_list_.Get());

    d3d_command_list_->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
        d3d_swap_chain_buffers_[current_back_buffer_].Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT));

    d3d_command_list_->Close();
    ID3D12CommandList* command_lists[] = { d3d_command_list_.Get() };
    d3d_command_queue_->ExecuteCommandLists(_countof(command_lists), command_lists);

    dxgi_swap_chain_->Present(0, 0);
    current_back_buffer_ = (current_back_buffer_ + 1) % kSwapChainBufferCount;

    frame_resource_manager_->curr_frame_resource()->fence = ++current_fence_value_;

    d3d_command_queue_->Signal(d3d_fence_.Get(), current_fence_value_);

    frame_resource_manager_->CirculateFrameResource(d3d_fence_.Get());

    std::wstring fps{ L"SandyHeroes(" };
    fps += std::to_wstring(client_timer_->Fps()) + L"fps)";
    SetWindowText(main_wnd_, fps.c_str());
}

void GameFramework::FlushCommandQueue()
{
    ++current_fence_value_;

    d3d_command_queue_->Signal(d3d_fence_.Get(), current_fence_value_);

    if (d3d_fence_->GetCompletedValue() < current_fence_value_)
    {
        HANDLE event_handle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);

        d3d_fence_->SetEventOnCompletion(current_fence_value_, event_handle);

        WaitForSingleObject(event_handle, INFINITE);
        CloseHandle(event_handle);
    }
}

D3D12_CPU_DESCRIPTOR_HANDLE GameFramework::CurrentBackBufferView() const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        d3d_rtv_heap_->GetCPUDescriptorHandleForHeapStart(), 
        current_back_buffer_,
        rtv_descriptor_size_);
}

D3D12_CPU_DESCRIPTOR_HANDLE GameFramework::DepthStencilView() const
{
    return d3d_dsv_heap_->GetCPUDescriptorHandleForHeapStart();
}

LRESULT GameFramework::ProcessWindowMessage(HWND h_wnd, UINT message_id, WPARAM w_param, LPARAM l_param)
{
    switch (message_id)
    {
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_KEYDOWN:
    case WM_KEYUP:
        input_manager_->EnQueueInputMessage(message_id, w_param, l_param, client_timer_->PlayTime());
        break;
    default:
        break;
    }
    return 0;
}

FrameResourceManager* GameFramework::frame_resource_manager() const
{
    return frame_resource_manager_.get();
}

DescriptorManager* GameFramework::descriptor_manager() const
{
    return descriptor_manager_.get();
}

HWND GameFramework::main_wnd() const
{
    return main_wnd_;
}

XMFLOAT2 GameFramework::client_size() const
{
    return XMFLOAT2{ (float)client_width_, (float)client_height_ };
}

SOCKET GameFramework::socket() const
{
    return socket_;
}

void GameFramework::ConnectServer()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    socket_ = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

    SOCKADDR_IN serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_ADDR, &serverAddr.sin_addr);

    int result = WSAConnect(socket_, (SOCKADDR*)&serverAddr, sizeof(serverAddr), NULL, NULL, NULL, NULL);

    recv_running_ = true;
    recv_thread_ = std::thread(&GameFramework::RecvThreadProc, this);
}

void GameFramework::do_send(void* p)
{
    unsigned char* packet = reinterpret_cast<unsigned char*>(p);
    int packet_size = packet[0];

    WSABUF buf;
    buf.buf = reinterpret_cast<CHAR*>(packet);
    buf.len = packet_size;

    DWORD sent = 0;
    WSASend(socket_, &buf, 1, &sent, 0, nullptr, nullptr);
}

void GameFramework::send_login_packet()
{
    // 로비없이 바로 인게임 테스트(나중에 변경 및 삭제)
    cs_packet_login p;
    p.size = sizeof(p);
    p.type = C2S_P_LOGIN;
    do_send(&p);
}

void GameFramework::send_mouse_move_packet(int x1, int x2)
{
    auto now = std::chrono::steady_clock::now();

    if (now - last_mouse_packet_time_ < mouse_packet_interval_)
        return;
    cs_packet_mouse_move mm;
    mm.size = sizeof(mm);
    mm.type = C2S_P_MOUSE_MOVE;
    mm.yaw = x1 - x2;
    do_send(&mm);
    last_mouse_packet_time_ = now;
}

void GameFramework::send_keyboard_input_packet(WPARAM w_param, bool is_press)
{
    cs_packet_keyboard_input ki;
    ki.size = sizeof(ki);
    ki.type = C2S_P_KEYBOARD_INPUT;
    ki.key = w_param;
    ki.pressed = is_press;
    do_send(&ki);
}


void GameFramework::ProcessPacket(char* p)
{
    static bool first_time = true;
    switch (p[1])
    {
    case S2C_P_USER_INFO:
    {
        sc_packet_user_info* packet = reinterpret_cast<sc_packet_user_info*>(p);
        AITestScene* ai_test_scene = dynamic_cast<AITestScene*>(scene_.get());
        if (ai_test_scene)
        {
            Object* player = ai_test_scene->player(); // BaseScene에 player_ 멤버가 존재함
            if (player)
            {
                player->set_position_vector(packet->x, packet->y, packet->z);
                player->set_id(packet->id);
            }
        }
    }
    break;
    case S2C_P_MOVE:
    {
        sc_packet_move* packet = reinterpret_cast<sc_packet_move*>(p);
        AITestScene* ai_test_scene = dynamic_cast<AITestScene*>(scene_.get());
        if (ai_test_scene)
        {

            Object* player = ai_test_scene->FindObject(packet->id);
            if (player && player->id()== packet->id)
            {
                XMFLOAT4X4 xf;
                memcpy(&xf, packet->matrix, sizeof(float) * 16);
                player->set_transform_matrix(xf);
            }
        }

    }
    break;
    case S2C_P_ENTER:
    {
        sc_packet_enter* packet = reinterpret_cast<sc_packet_enter*>(p);
        AITestScene* ai_test_scene = dynamic_cast<AITestScene*>(scene_.get());
        if (ai_test_scene)
        {
            ai_test_scene->AddRemotePlayer(packet->id, packet->name, XMFLOAT3(packet->x, packet->y, packet->z));
        }
    }
    break;
    case S2C_P_LEAVE:
    {
        cs_packet_login* packet = reinterpret_cast<cs_packet_login*>(p);

        break;
    }
    default:
        break;
    }
}


//void GameFramework::do_recv()
//{
//    DWORD recv_bytes = 0;
//    DWORD flags = 0;
//
//    ZeroMemory(&recv_over_.over, sizeof(recv_over_.over));
//    recv_over_.wsa_buf.buf = recv_over_.buffer;
//    recv_over_.wsa_buf.len = sizeof(recv_over_.buffer);
//    recv_over_.flags = 0;
//
//    WSARecv(socket_, &recv_over_.wsa_buf, 1, &recv_bytes, &recv_over_.flags, &recv_over_.over, nullptr);
//}

//void GameFramework::CheckRecv()
//{
//    DWORD bytes = 0;
//    DWORD flags = 0;
//
//    BOOL ret = WSAGetOverlappedResult(socket_, &recv_over_.over, &bytes, FALSE, &flags);
//    if (ret && bytes > 0)
//    {
//        char* buffer = recv_over_.buffer;
//        int total_size = bytes;
//
//        int processed = 0;
//        while (processed < total_size)
//        {
//            unsigned char packet_size = buffer[processed];
//            if (processed + packet_size > total_size) break;
//
//            ProcessPacket(&buffer[processed]);
//            processed += packet_size;
//        }
//
//        do_recv();
//    }
//}


void GameFramework::RecvThreadProc()
{
    while (recv_running_)
    {
        DWORD bytes = 0;
        DWORD flags = 0;

        ZeroMemory(&recv_over_.over, sizeof(recv_over_.over));
        recv_over_.wsa_buf.buf = recv_over_.buffer;
        recv_over_.wsa_buf.len = sizeof(recv_over_.buffer);
        recv_over_.flags = 0;

        int ret = WSARecv(socket_, &recv_over_.wsa_buf, 1, &bytes, &recv_over_.flags, &recv_over_.over, NULL);
        if (ret == SOCKET_ERROR)
        {
            int err = WSAGetLastError();
            if (err != WSA_IO_PENDING)
            {
                break;
            }
        }

        BOOL result = WSAGetOverlappedResult(socket_, &recv_over_.over, &bytes, TRUE, &flags);
        if (result && bytes > 0)
        {
            char* buffer = recv_over_.buffer;
            int total_size = bytes;
            int processed = 0;

            while (processed < total_size)
            {
                unsigned char packet_size = buffer[processed];
                if (processed + packet_size > total_size) break;

                ProcessPacket(&buffer[processed]);
                processed += packet_size;
            }
        }
    }
}