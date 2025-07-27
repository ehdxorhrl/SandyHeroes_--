#pragma once

#include "stdafx.h"

template<typename T>
class UploadBuffer
{
public:
	UploadBuffer(ID3D12Device* device, UINT element_count, bool is_constant_buffer) :
		is_constant_buffer_(is_constant_buffer)
	{
		element_byte_size_ = sizeof(T);

		if (is_constant_buffer)
			element_byte_size_ = d3d_util::CalculateConstantBufferSize(element_byte_size_);

		device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(element_byte_size_ * element_count),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(d3d_buffer_.GetAddressOf()));

		d3d_buffer_->Map(0, nullptr, reinterpret_cast<void**>(&mapped_data_));

	}
	UploadBuffer(const UploadBuffer& rhs) = delete;
	UploadBuffer& operator=(const UploadBuffer& rhs) = delete;

	~UploadBuffer()
	{
		if (d3d_buffer_)
			d3d_buffer_->Unmap(0, nullptr);

		mapped_data_ = nullptr;
	}

	ID3D12Resource* Resource() const
	{
		return d3d_buffer_.Get();
	}

	void CopyData(int element_index, const T& data)
	{
		memcpy(&mapped_data_[element_byte_size_ * element_index], &data, sizeof(T));
	}

private:
	ComPtr<ID3D12Resource> d3d_buffer_ = nullptr;
	BYTE* mapped_data_ = nullptr;

	UINT element_byte_size_ = 0;
	bool is_constant_buffer_ = false;
};
