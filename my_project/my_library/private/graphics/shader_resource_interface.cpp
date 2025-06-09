#include "graphics/shader_resource_interface.h"
#include "graphics/graphics_engine.h"

ICShaderResource::ICShaderResource()
{

}

ICShaderResource::~ICShaderResource()
{
	Release();
}

/// @brief リソース作成
/// @param _size サイズ
/// @return 成否
bool ICShaderResource::CreateResource(UINT64 _size)
{
	if (m_resource != nullptr)
	{
		printf("ICShaderResource::CreateResource リソースはすでに生成済みです\n");
		return true;
	}

	ID3D12Device* d3dDevice = CGraphicsEngine::GetInstance().GetD3dDevice();
	if (d3dDevice == nullptr)
	{
		printf("ICShaderResource::CreateResource d3dDeviceがnullです\n");
		return false;
	}

	// ヒーププロパティ
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.CreationNodeMask = 1;
	heapProp.VisibleNodeMask = 1;

	// リソース状態
	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = _size;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	HRESULT result = d3dDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_resource));
	if (FAILED(result))
	{
		printf("ICShaderResource::CreateResource リソースの作成に失敗。\n");
		return false;
	}

	return true;
}

/// @brief バッファコピー
/// @param _dataPtr データへのポインタ
/// @param _size サイズ
/// @return 成否
bool ICShaderResource::CopyBuffer(const void* _dataPtr, UINT64 _size)
{
	if (m_resource == nullptr)
	{
		printf("ICShaderResource::CopyBuffer m_resourceがnullです\n");
		return false;
	}

	// バッファコピー
	void* bufferPtr = nullptr;
	HRESULT result = m_resource->Map(0, nullptr, reinterpret_cast<void**>(&bufferPtr));
	if (FAILED(result))
	{
		printf("ICShaderResource::CopyBuffer バッファのマップに失敗\n");
		return false;
	}

	// メモリコピー
	memcpy_s(bufferPtr, _size, _dataPtr, _size);
	m_resource->Unmap(0, nullptr);

	return true;

}

/// @brief 開放
void ICShaderResource::Release()
{
	if (m_resource != nullptr)
	{
		m_resource->Release();
		m_resource = nullptr;
	}
}
