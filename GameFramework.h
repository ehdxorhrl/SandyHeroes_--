#pragma once

class Timer;
class Scene;
class FrameResourceManager;
class DescriptorManager;
class InputManager;

struct EXP_OVER
{
	WSAOVERLAPPED over = {};
	WSABUF wsa_buf = {};
	char buffer[MAX_PATH] = {}; // ũ�� �˳��ϰ�
	DWORD flags = 0;

	EXP_OVER()
	{
		ZeroMemory(&over, sizeof(over));
		wsa_buf.buf = buffer;
		wsa_buf.len = sizeof(buffer);
	}
};

class GameFramework
{
public:
	GameFramework();
	~GameFramework();

	void Initialize(HINSTANCE hinstance, HWND hwnd);

	void InitDirect3D();
	void CreateCommandObject();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();

	void BuildRootSignature();

	void OnResize();

	void ChangeWindowMode();

	void ProcessInput();
	void ProcessInput(UINT id, WPARAM w_param, LPARAM l_param, float time);

	void FrameAdvance();

	// gpu, cpu ����ȭ
	void FlushCommandQueue(); 

	//���� �ĸ���� ���� CPU�ڵ��� ����
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

	LRESULT CALLBACK ProcessWindowMessage(HWND h_wnd, UINT message_id, WPARAM w_param, LPARAM l_param);

	//getter
	FrameResourceManager* frame_resource_manager() const;
	DescriptorManager* descriptor_manager() const;
	HWND main_wnd() const;
	XMFLOAT2 client_size() const;
	SOCKET socket() const; // ����

	//���� ����
	void ConnectServer();
	//��Ŷ ����
	void send_login_packet();
	void send_mouse_move_packet(int x1, int x2);
	void send_keyboard_input_packet(WPARAM w_param, bool is_press);
	void ProcessPacket(char* p);
	//void do_recv();
	//void CheckRecv();
	void RecvThreadProc();
	void do_send(void* p);
	

private:
	static GameFramework* kGameFramework;

	static const UINT kInitFenceValue = 0;			// �潺 �ʱⰪ
	static const UINT kSwapChainBufferCount = 2;	// ����ü�� ���� ��
	static const UINT kDepthStencilBufferCount = 2;

	static constexpr UINT SHADOW_MAP_SIZE = 4096;

	HINSTANCE app_instance_ = nullptr;	// ���ø����̼� �ν��Ͻ� �ڵ�
	HWND main_wnd_ = nullptr;			// ���� ������ �ڵ�

	ComPtr<IDXGIFactory4> dxgi_factory_;

	ComPtr<ID3D12Device> d3d_device_;
	ComPtr<ID3D12Fence> d3d_fence_;
	UINT64 current_fence_value_ = kInitFenceValue;

	ComPtr<ID3D12CommandQueue> d3d_command_queue_;
	ComPtr<ID3D12CommandAllocator> d3d_command_allocator_;
	ComPtr<ID3D12GraphicsCommandList> d3d_command_list_;

	ComPtr<IDXGISwapChain3> dxgi_swap_chain_;
	std::array<ComPtr<ID3D12Resource>, kSwapChainBufferCount> d3d_swap_chain_buffers_;

	ComPtr<ID3D12DescriptorHeap> d3d_rtv_heap_;
	ComPtr<ID3D12DescriptorHeap> d3d_dsv_heap_;

	ComPtr<ID3D12Resource> d3d_depth_stencil_buffer_;
	ComPtr<ID3D12Resource> d3d_shadow_depth_buffer_;

	ComPtr<ID3D12RootSignature> d3d_root_signature_;

	UINT rtv_descriptor_size_;
	UINT dsv_descriptor_size_;
	UINT cbv_srv_uav_descriptor_size_;

	UINT msaa_quality_; // 4x msaa ����Ƽ ����
	bool msaa_state_ = false; // 4x msaa�� ����ϴ��� ����

	DXGI_FORMAT back_buffer_format_ = DXGI_FORMAT_R8G8B8A8_UNORM;
	UINT current_back_buffer_ = 0; // ���� �ĸ������ �ε���

	DXGI_FORMAT depth_stencil_buffer_format_ = DXGI_FORMAT_D24_UNORM_S8_UINT;

	int client_width_ = kDefaultFrameBufferWidth;
	int client_height_ = kDefaultFrameBufferHeight;
	int client_refresh_rate_ = kDefaultRefreshRate;
	BOOL client_full_screen_state_ = FALSE; // ��üȭ�� ����

	D3D12_VIEWPORT client_viewport_;
	D3D12_RECT scissor_rect_;

	D3D12_VIEWPORT shadow_viewport_;
	D3D12_RECT shadow_scissor_rect_;

	std::unique_ptr<Timer> client_timer_;

	std::unique_ptr<Scene> scene_ = nullptr;

	std::unique_ptr<FrameResourceManager> frame_resource_manager_ = nullptr;
	std::unique_ptr<DescriptorManager> descriptor_manager_ = nullptr;
	std::unique_ptr<InputManager> input_manager_ = nullptr;

	std::chrono::steady_clock::time_point last_mouse_packet_time_;

	const std::chrono::milliseconds mouse_packet_interval_{ 17 }; // �� 60fps
	bool is_initialized_ = false;
private:
	SOCKET socket_;
	long long id_;
	EXP_OVER recv_over_;
	std::thread recv_thread_;
	bool recv_running_;
};

