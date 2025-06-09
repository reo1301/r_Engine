#pragma once
#include <d3d12.h>

MY_LIB_NAMESPACE_BEGIN

class CGraphicsCommandWrapper
{
public:
	CGraphicsCommandWrapper();
	~CGraphicsCommandWrapper();

	/// @brief ������
	/// @return ����
	bool Initialize();

	/// @brief �I��
	void Finalize();

	/// @brief ���Z�b�g
	void Reset();

	/// @brief ���s
	void Execute();

	/// @brief ���\�[�X�o���A
	/// @param _resource �o���A�����s���郊�\�[�X
	/// @param _prev �O���
	/// @param _next �����
	void ResourceBarrier(ID3D12Resource* _resource, D3D12_RESOURCE_STATES _prev, D3D12_RESOURCE_STATES _next);

	/// @brief �R�}���h�L���[���擾
	/// @return �R�}���h�L���[
	ID3D12CommandQueue* GetCommandQueue();

	/// @brief �R�}���h�A���P�[�^���擾
	/// @return �R�}���h�A���P�[�^
	ID3D12CommandAllocator* GetCommandAllocator();

	/// @brief �R�}���h���X�g���擾
	/// @return �R�}���h���X�g
	ID3D12GraphicsCommandList* GetCommandList();

private:
	/// @brief �J��
	void Release();

	/// @brief �R�}���h�L���[���쐬
	/// @return ����
	bool CreateD3d12CommandQueue();

	/// @brief �R�}���h�A���P�[�^���쐬
	/// @return ����
	bool CreateD3d12CommandAllocator();

	/// @brief �R�}���h���X�g���쐬
	/// @return ����
	bool CreateD3d12CommandList();

private:
	ID3D12CommandQueue* m_commandQueue = nullptr;				// �R�}���h�L���[
	ID3D12CommandAllocator* m_commandAllocator = nullptr;		// �R�}���h�A���P�[�^
	ID3D12GraphicsCommandList* m_commandList = nullptr;			// �R�}���h���X�g
};

MY_LIB_NAMESPACE_END
