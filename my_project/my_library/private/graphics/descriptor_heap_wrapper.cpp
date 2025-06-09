#include "graphics/descriptor_heap_wrapper.h"
#include "graphics/graphics_engine.h"

CGraphicsDescriptorHeapWrapper::CGraphicsDescriptorHeapWrapper()
{

}

CGraphicsDescriptorHeapWrapper::~CGraphicsDescriptorHeapWrapper()
{

}

/// @brief ������
/// @param _initData �������f�[�^
/// @return ����
bool CGraphicsDescriptorHeapWrapper::Initialize(const InitData& _initData)
{
	if (!CreateDescriptorHeap(_initData))
	{
		printf("CGraphicsDescriptorHeapWrapper::Initialize �f�B�X�N���v�^�q�[�v�̍쐬�Ɏ��s\n");
		return false;
	}

	return true;
}

/// @brief �I��
void CGraphicsDescriptorHeapWrapper::Finalize()
{
	Release();
}

/// @brief �f�B�X�N���v�^�q�[�v�̎擾
/// @return �f�B�X�N���v�^�q�[�v
ID3D12DescriptorHeap* CGraphicsDescriptorHeapWrapper::GetDescriptorHeap()
{
	return m_descriptorHeap;
}

/// @brief �J��
void CGraphicsDescriptorHeapWrapper::Release()
{
	if (m_descriptorHeap != nullptr)
	{
		m_descriptorHeap->Release();
		m_descriptorHeap = nullptr;
	}
}

/// @brief �f�B�X�N���v�^�q�[�v
/// @param _initData �������f�[�^
/// @return ����
bool CGraphicsDescriptorHeapWrapper::CreateDescriptorHeap(const InitData& _initData)
{
	if (m_descriptorHeap != nullptr)
	{
		printf("CGraphicsDescriptorHeapWrapper::CreateDescriptorHeap �f�B�X�N���v�^�q�[�v�͐����ς�\n");
		return true;
	}

	ID3D12Device* d3dDevice = CGraphicsEngine::GetInstance().GetD3dDevice();

	if (d3dDevice == nullptr)
	{
		printf("CGraphicsDescriptorHeapWrapper::CreateDescriptorHeap _d3dDevice��null�ł�\n");
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
		printf("CGraphicsEngine::CreateD3d12DescriptorHeap �f�B�X�N���v�^�q�[�v�̐����Ɏ��s\n");
		return false;
	}

	return true;
}
