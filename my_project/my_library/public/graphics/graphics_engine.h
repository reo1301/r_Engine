#pragma once

//�C���N���[�h
//�R�s�[�֎~
#include "../noncopyable.h"

//D3D�n
#include <d3d12.h>
#include <dxgi1_4.h>

MY_LIB_NAMESPACE_BEGIN

#define RTV_NUM	2		//�����_�[�^�[�Q�b�g�̐�(�t���[���o�b�t�@�ƃo�b�N�o�b�t�@��2)

class CGraphicsEngine : private Noncopyable
{
private:
	/// @brief �R���X�g���N�^
	CGraphicsEngine();

public:
	/// @brief �f�X�g���N�^
	~CGraphicsEngine();

public:
	/// @brief ����������
	void Initialize();

	/// @brief �I������
	void Finalize();

	/// @brief �X�V
	void Update();

private:
#ifdef _DEBUG

	/// @brief �f�o�b�O���C���[�̏�����
	void InitDebugLayer() const;

#endif

	/// @brief DXGI�I�u�W�F�N�g�̐����Ɏg�p����DXGI�t�@�N�g���쐬
	void CreateDxgiFactory();

	/// @brief D3D12�f�o�C�X���쐬
	void CreateD3d12Device();

	/// @brief �R�}���h�L���[���쐬
	void CreateD3d12CommandQueue();

	/// @brief �X���b�v�`�F�[�����쐬
	void CreateSwapChain();

	/// @brief ���[�g�V�O�l�`�����쐬
	void CreateRootSignature();

	/// @brief �R�}���h�A���P�[�^���쐬
	void CreateD3d12CommandAllocator();

	/// @brief �R�}���h���X�g���쐬
	void CreateD3d12CommandList();

	/// @brief RTV�p�f�B�X�N���v�^�q�[�v���쐬
	void CreateD3d12RtvDescriptorHeap();

	/// @brief �����_�[�^�[�Q�b�g�̍쐬
	void CreateRenderTarget();

	/// @brief �t�F���X�̍쐬
	void CreateD3d12Fence();

	/// @brief �����_�[�^�[�Q�b�g�̃Z�b�g
	void SetRenderTarget();

	/// @brief �o�b�N�o�b�t�@�̃N���A
	void ClearBackBuffer();

	/// @brief �`��ҋ@
	void WaitDraw();

private:
	static CGraphicsEngine* s_instance;		//�V���O���g���̃C���X�^���X

public:
	/// @brief �V���O���g���̃C���X�^���X����
	static void CreateInstance();

	/// @brief �V���O���g���̃C���X�^���X�擾
	/// @return GraphicsEngine�̃C���X�^���X�̎Q��
	static CGraphicsEngine& GetInstance();

	/// @brief �V���O���g���̃C���X�^���X�폜
	static void Delete();

private:
	static constexpr float RT_CLEAR_COLOR[4] = { 0.7f, 0.7f, 0.7f, 1.0f };

private:
	IDXGIFactory4* m_dxgiFactory = nullptr;									//DXGI�t�@�N�g��
	ID3D12Device* m_d3d12Device = nullptr;									//D3D12�f�o�C�X
	ID3D12CommandQueue* m_d3d12CommandQueue = nullptr;						//�R�}���h�L���[
	IDXGISwapChain3* m_dxgiSwapChain = nullptr;								//�X���b�v�`�F�[��
	ID3D12RootSignature* m_d3d12RootSignature = nullptr;					//���[�g�V�O�l�`��
	ID3D12CommandAllocator* m_d3d12CommandAllocator = nullptr;				//�R�}���h�A���P�[�^
	ID3D12GraphicsCommandList* m_d3d12GraphicsCommandList = nullptr;		//�R�}���h���X�g
	ID3D12DescriptorHeap* m_d3d12RtvDescriptorHeap = nullptr;				//RTV�p�f�B�X�N���v�^�q�[�v
	ID3D12Resource* m_renderTarget[RTV_NUM] = {};							//�����_�[�^�[�Q�b�g
	ID3D12Fence* m_d3d12Fence = nullptr;									// �t�F���X
	HANDLE m_fenceEvent = nullptr;											// �t�F���X�C�x���g
	unsigned long long m_fenceValue = 0;									// �t�F���X�̌��݂̒l
	unsigned int m_frameIdx = 0;											//���݃t���[���o�b�t�@�̃C���f�b�N�X
};

MY_LIB_NAMESPACE_END