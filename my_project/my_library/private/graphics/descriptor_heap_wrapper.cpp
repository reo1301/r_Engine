#include "graphics/descriptor_heap_wrapper.h"
#include "graphics/graphics_engine.h"

CGraphicsDescriptorHeapWrapper::CGraphicsDescriptorHeapWrapper()
{

}

CGraphicsDescriptorHeapWrapper::~CGraphicsDescriptorHeapWrapper()
{

}

/// @brief 初期化
/// @param _initData 初期化データ
/// @return 成否
bool CGraphicsDescriptorHeapWrapper::Initialize(const InitData& _initData)
{
	if (!CreateDescriptorHeap(_initData))
	{
		printf("CGraphicsDescriptorHeapWrapper::Initialize ディスクリプタヒープの作成に失敗\n");
		return false;
	}

	return true;
}

/// @brief 終了
void CGraphicsDescriptorHeapWrapper::Finalize()
{
	Release();
}

/// @brief ディスクリプタヒープの取得
/// @return ディスクリプタヒープ
ID3D12DescriptorHeap* CGraphicsDescriptorHeapWrapper::GetDescriptorHeap()
{
	return m_descriptorHeap;
}

/// @brief 開放
void CGraphicsDescriptorHeapWrapper::Release()
{
	if (m_descriptorHeap != nullptr)
	{
		m_descriptorHeap->Release();
		m_descriptorHeap = nullptr;
	}
}

/// @brief ディスクリプタヒープ
/// @param _initData 初期化データ
/// @return 成否
bool CGraphicsDescriptorHeapWrapper::CreateDescriptorHeap(const InitData& _initData)
{
	if (m_descriptorHeap != nullptr)
	{
		printf("CGraphicsDescriptorHeapWrapper::CreateDescriptorHeap ディスクリプタヒープは生成済み\n");
		return true;
	}

	ID3D12Device* d3dDevice = CGraphicsEngine::GetInstance().GetD3dDevice();

	if (d3dDevice == nullptr)
	{
		printf("CGraphicsDescriptorHeapWrapper::CreateDescriptorHeap _d3dDeviceがnullです\n");
		return false;
	}

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = _initData.heapType;
	desc.NumDescriptors = _initData.numDescriptor;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	desc.NodeMask = 0;

	HRESULT result;
	result = d3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_descriptorHeap));
	if (result != S_OK || m_descriptorHeap == nullptr)
	{
		printf("CGraphicsEngine::CreateD3d12DescriptorHeap ディスクリプタヒープの生成に失敗\n");
		return false;
	}

	return true;
}
