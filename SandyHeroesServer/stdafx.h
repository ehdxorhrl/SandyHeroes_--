// header.h: ǥ�� �ý��� ���� ����
// �Ǵ� ������Ʈ Ư�� ���� ������ ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#define NOMINMAX						// min, max ��ũ�θ� ������� ����
// Windows ��� ����
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <MSWSock.h>

#pragma comment (lib, "WS2_32.lib")
#pragma comment (lib, "MSWSock.lib")
// C ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <tchar.h>
#include <assert.h>
#include <concurrent_unordered_map.h>

// C++ ��� ����
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <array>
#include <unordered_map>
#include <limits>
#include <numeric>
#include <memory>
#include <functional>
#include <random>
#include <chrono>         // �ð� ����
#include <thread>
#include <atomic>
#include <mutex>

// DirectX ���� ��� ���� �� ����
#include <wrl.h>
#include <shellapi.h>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#include "Packet.h"

#include "d3dx12.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

//���� �ӽ�
static std::random_device kRandomDevice;
static std::mt19937 kRandomGenerator(kRandomDevice());

// �����
//constexpr int kDefaultFrameBufferWidth = 1366;
//constexpr int kDefaultFrameBufferHeight = 768;
//constexpr UINT kDefaultRefreshRate = 60;
//constexpr int kMaxBoneCount = 128; //skinned mesh�� �� �ִ� ����
//constexpr int kMaxLights = 16;		// ����ó���� �ִ� ����

//enum class RootParameterIndex {
//	kWorldMatrix = 0, kBoneTransform, kBoneOffset, kRenderPass,
//	kMaterial, kAlbedoMap, kSpecGlosMap, kMetalGlosMap, kEmissionMap, kNormalMap, kCubeMap
//};

enum IO_OP { IO_RECV, IO_SEND, IO_ACCEPT };

// ���� ���� �Լ�
