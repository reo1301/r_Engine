#pragma once
#include <d3d12.h>

MY_LIB_NAMESPACE_BEGIN

class CGraphicsDescriptorHeapWrapper
{
public:
	struct InitData
	{
		D3D12_DESCRIPTOR_HEAP_TYPE heapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;		// �q�[�v�^�C�v
		unsigned int numDescriptor = 0;			// �f�B�X�N���v�^��
	};

public:
	CGraphicsDescriptorHeapWrapper();
	~CGraphicsDescriptorHeapWrapper();

	/// @brief ������
	/// @param _initData �������f�[�^
	/// @return ����
	bool Initialize(const InitData& _initData);

	/// @brief �I��
	void Finalize();

	/// @brief �f�B�X�N���v�^�q�[�v�̎擾
	/// @return �f�B�X�N���v�^�q�[�v
	ID3D12DescriptorHeap* GetDescriptorHeap();

private:
	/// @brief �J��
	void Release();

	/// @brief �f�B�X�N���v�^�q�[�v
	/// @param _initData �������f�[�^
	/// @return ����
	bool CreateDescriptorHeap(const InitData& _initData);

private:
	ID3D12DescriptorHeap* m_descriptorHeap = nullptr;		// �f�B�X�N���v�^�q�[�v

};

MY_LIB_NAMESPACE_END
