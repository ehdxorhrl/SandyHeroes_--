// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define NOMINMAX						// min, max 메크로를 사용하지 않음
// Windows 헤더 파일
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <MSWSock.h>

#pragma comment (lib, "WS2_32.lib")
#pragma comment (lib, "MSWSock.lib")
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <tchar.h>
#include <assert.h>
#include <concurrent_unordered_map.h>

// C++ 헤더 파일
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
#include <chrono>         // 시간 관련
#include <thread>
#include <atomic>
#include <mutex>

// DirectX 관련 헤더 파일 및 선언문
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

//랜덤 머신
static std::random_device kRandomDevice;
static std::mt19937 kRandomGenerator(kRandomDevice());

// 상수값
//constexpr int kDefaultFrameBufferWidth = 1366;
//constexpr int kDefaultFrameBufferHeight = 768;
//constexpr UINT kDefaultRefreshRate = 60;
//constexpr int kMaxBoneCount = 128; //skinned mesh의 본 최대 개수
//constexpr int kMaxLights = 16;		// 조명처리의 최대 개수

//enum class RootParameterIndex {
//	kWorldMatrix = 0, kBoneTransform, kBoneOffset, kRenderPass,
//	kMaterial, kAlbedoMap, kSpecGlosMap, kMetalGlosMap, kEmissionMap, kNormalMap, kCubeMap
//};

enum IO_OP { IO_RECV, IO_SEND, IO_ACCEPT };

// 오류 검출 함수
