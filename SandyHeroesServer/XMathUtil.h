#pragma once
// Windows ���� �Լ� (e.g., OutputDebugString)
#include <windows.h>

// DirectX ����
#include <DirectXMath.h>
#include <DirectXCollision.h> // �ʿ� ��

// ǥ�� ���̺귯��
#include <cmath>       // fabs
#include <limits>      // numeric_limits
#include <string>      // string, wstring
#include <fstream>     // ifstream
#include <vector>      // vector
#include <list>        // list (�ʿ� ��)
#include <cassert>     // assert (�����)
#include <cstdint>     // BYTE ��


using namespace DirectX;

inline bool IsZero(float value, float epsilon = std::numeric_limits<float>::epsilon() * 100)
{
	return std::fabs(value) < epsilon;
}

//namespace d3d_util
//{
//	// ���ε� ���۸� �̿��Ͽ� ����Ʈ���� ����
//	// ����: �� �Լ��� ����ǰ� gpu ����� ���� ����� �� upload_buffer�� �����ؾ��Ѵ�.
//	ComPtr<ID3D12Resource> CreateDefaultBuffer(
//		ID3D12Device* device,
//		ID3D12GraphicsCommandList* command_list,
//		const void* init_data,
//		UINT64 byte_size,
//		ComPtr<ID3D12Resource>& upload_buffer);
//
//	// ��� ���۴� �׻� 256����Ʈ�� ������� �Ѵ�.
//	// �� �Լ��� ������ ����� 256�� ����� ������ִ� �Լ���.
//	UINT CalculateConstantBufferSize(UINT byte_size);
//}

namespace xmath_util_float3
{
	inline XMFLOAT3 Add(const XMFLOAT3& vector1, const XMFLOAT3& vector2)
	{
		XMFLOAT3 r_value;
		XMStoreFloat3(&r_value, XMLoadFloat3(&vector1) + XMLoadFloat3(&vector2));
		return r_value;
	}

	inline XMFLOAT3 Subtract(const XMFLOAT3& vector1, const XMFLOAT3& vector2)
	{
		XMFLOAT3 r_value;
		XMStoreFloat3(&r_value, XMLoadFloat3(&vector1) - XMLoadFloat3(&vector2));
		return r_value;
	}

	inline XMFLOAT3 ScalarProduct(const XMFLOAT3& vector, float scalar)
	{
		XMFLOAT3 r_value;
		XMStoreFloat3(&r_value, XMLoadFloat3(&vector) * scalar);
		return r_value;
	}

	inline float DotProduct(const XMFLOAT3& vector1, const XMFLOAT3& vector2)
	{
		XMFLOAT3 r_value;
		XMStoreFloat3(&r_value, XMVector3Dot(XMLoadFloat3(&vector1), XMLoadFloat3(&vector2)));
		return r_value.x;
	}

	inline XMFLOAT3 CrossProduct(const XMFLOAT3& vector1, const XMFLOAT3& vector2)
	{
		XMFLOAT3 r_value;
		XMStoreFloat3(&r_value, XMVector3Cross(XMLoadFloat3(&vector1), XMLoadFloat3(&vector2)));
		return r_value;
	}

	inline XMFLOAT3 Normalize(const XMFLOAT3& vector)
	{
		XMFLOAT3 r_value;
		XMStoreFloat3(&r_value, XMVector3Normalize(XMLoadFloat3(&vector)));
		return r_value;
	}

	inline float Length(const XMFLOAT3& vector)
	{
		XMFLOAT3 r_value;
		XMStoreFloat3(&r_value, XMVector3Length(XMLoadFloat3(&vector)));
		return r_value.x;
	}

	inline float AngleBetween(const XMFLOAT3& vector1, const XMFLOAT3& vector2)
	{
		XMFLOAT3 r_value;
		XMStoreFloat3(&r_value, XMVector3AngleBetweenNormals(XMLoadFloat3(&vector1), XMLoadFloat3(&vector2)));
		return r_value.x;
	}

	inline float LengthSq(const XMFLOAT3& v)
	{
		return v.x * v.x + v.y * v.y + v.z * v.z;
	}

}

namespace xmath_util_float4
{
	inline XMFLOAT4 Add(const XMFLOAT4& vector1, const XMFLOAT4& vector2)
	{
		XMFLOAT4 r_value;
		XMStoreFloat4(&r_value, XMLoadFloat4(&vector1) + XMLoadFloat4(&vector2));
		return r_value;
	}

	inline XMFLOAT4 Subtract(const XMFLOAT4& vector1, const XMFLOAT4& vector2)
	{
		XMFLOAT4 r_value;
		XMStoreFloat4(&r_value, XMLoadFloat4(&vector1) - XMLoadFloat4(&vector2));
		return r_value;
	}

	inline XMFLOAT4 ScalarProduct(const XMFLOAT4& vector, float scalar)
	{
		XMFLOAT4 r_value;
		XMStoreFloat4(&r_value, XMLoadFloat4(&vector) * scalar);
		return r_value;
	}

}

namespace xmath_util_float4x4
{
	inline XMFLOAT4X4 Identity()
	{
		XMFLOAT4X4 r_value;
		XMStoreFloat4x4(&r_value, XMMatrixIdentity());
		return r_value;
	}

	inline XMFLOAT4X4 Add(const XMFLOAT4X4& matrix1, const XMFLOAT4X4& matrix2)
	{
		XMFLOAT4X4 r_value;
		XMStoreFloat4x4(&r_value, XMLoadFloat4x4(&matrix1) + XMLoadFloat4x4(&matrix2));
		return(r_value);
	}

	inline XMFLOAT4X4 PerspectiveFovLH(float fov_angle_y, float aspect_ratio, float near_z, float far_z)
	{
		XMFLOAT4X4 r_value;
		XMStoreFloat4x4(&r_value, XMMatrixPerspectiveFovLH(fov_angle_y, aspect_ratio, near_z, far_z));
		return(r_value);
	}

	inline XMFLOAT4X4 Multiply(const XMFLOAT4X4& matrix1, const XMFLOAT4X4& matrix2)
	{
		XMFLOAT4X4 r_value;
		XMStoreFloat4x4(&r_value, XMLoadFloat4x4(&matrix1) * XMLoadFloat4x4(&matrix2));
		return r_value;
	}

	inline XMFLOAT4X4 TransPose(const XMFLOAT4X4& matrix)
	{
		XMFLOAT4X4 r_value;
		XMStoreFloat4x4(&r_value, XMMatrixTranspose(XMLoadFloat4x4(&matrix)));
		return r_value;
	}

	inline XMFLOAT4X4 ScalarProduct(const XMFLOAT4X4& matrix, float scalar)
	{
		XMFLOAT4X4 r_value;
		XMStoreFloat4x4(&r_value, XMLoadFloat4x4(&matrix) * scalar);
		return r_value;
	}

	inline XMFLOAT4X4 Interpolate(const XMFLOAT4X4& matrix1, const XMFLOAT4X4& matrix2, float t)
	{
		XMFLOAT4X4 r_value;
		XMVECTOR s1, r1, t1;
		XMVECTOR s2, r2, t2;
		XMMatrixDecompose(&s1, &r1, &t1, XMLoadFloat4x4(&matrix1));
		XMMatrixDecompose(&s2, &r2, &t2, XMLoadFloat4x4(&matrix2));
		XMVECTOR S = XMVectorLerp(s1, s2, t);
		XMVECTOR R = XMQuaternionSlerp(r1, r2, t);
		XMVECTOR T = XMVectorLerp(t1, t2, t);

		XMStoreFloat4x4(&r_value, XMMatrixAffineTransformation(S, XMVectorZero(), R, T));
		return r_value;
	}


}

// xmf ���� ������ �����ε�
inline XMFLOAT3 operator+(const XMFLOAT3& lhs, const XMFLOAT3& rhs) { return xmath_util_float3::Add(lhs, rhs); }
inline void operator+=(XMFLOAT3& lhs, const XMFLOAT3& rhs) { lhs = xmath_util_float3::Add(lhs, rhs); }
inline XMFLOAT3 operator-(const XMFLOAT3& lhs, const XMFLOAT3& rhs) { return xmath_util_float3::Subtract(lhs, rhs); }
inline void operator-=(XMFLOAT3& lhs, const XMFLOAT3& rhs) { lhs = xmath_util_float3::Subtract(lhs, rhs); }
inline XMFLOAT3 operator*(const XMFLOAT3& lhs, const float& rhs) { return xmath_util_float3::ScalarProduct(lhs, rhs); }
inline XMFLOAT4X4 operator*(const XMFLOAT4X4& lhs, const XMFLOAT4X4& rhs) { return xmath_util_float4x4::Multiply(lhs, rhs); }

namespace file_load_util
{
#ifdef _DEBUG
	// �ε� ��ū�� �߸��Ǿ����� ����׿� ������ ������ִ� �Լ�
	inline void PrintDebugStringLoadTokenError(const std::string& file_name,
		const std::string& load_token, const std::string& compare_token)
	{
		std::cout << "[DEBUG] load_token: " << load_token << std::endl;
		if (load_token != compare_token)
		{
			std::string temp = file_name + "�� �ε��ϴ� �������� " + load_token + "�κп� ������ ������ϴ�.";
			std::wstring debug_str;
			debug_str.assign(temp.begin(), temp.end());
			OutputDebugString(debug_str.c_str());
			throw;
		}

	}
#endif // DEBUG

	inline BYTE ReadStringFromFile(std::ifstream& file, std::string& str)
	{
		BYTE strLength = 0;
		file.read((char*)&strLength, sizeof(BYTE));
		str.resize(strLength);
		file.read(&str[0], strLength);
		return strLength;
	}

	template <class T>
	inline T ReadFromFile(std::ifstream& file)
	{
		T rvalue;
		file.read((char*)&rvalue, sizeof(T));
		return rvalue;
	}

	template <class T>
	inline void ReadFromFile(std::ifstream& file, T* buffer, const int& count)
	{
		file.read((char*)buffer, sizeof(T) * count);
	}
}