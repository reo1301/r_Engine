#include "graphics/graphics_command_wrapper.h"

CGraphicsCommandWrapper::CGraphicsCommandWrapper()
{
}

CGraphicsCommandWrapper::~CGraphicsCommandWrapper()
{
	Finalize();
}

/// @brief ������
/// @param _d3dDevice d3d�f�o�C�X
/// @return ����
bool CGraphicsCommandWrapper::Initialize(ID3D12Device* _d3dDevice)
{
	// �R�}���h�L���[���쐬
	if (!CreateD3d12CommandQueue(_d3dDevice))
	{
		printf("CGraphicsCommandWrapper::Initialize �R�}���h�L���[�̍쐬�Ɏ��s\n");
		return false;
	}

	// �R�}���h�A���P�[�^���쐬
	if (!CreateD3d12CommandAllocator(_d3dDevice))
	{
		printf("CGraphicsCommandWrapper::Initialize �R�}���h�A���P�[�^�̍쐬�Ɏ��s\n");
		return false;
	}

	// �R�}���h���X�g���쐬
	if (!CreateD3d12CommandList(_d3dDevice))
	{
		printf("CGraphicsCommandWrapper::Initialize �R�}���h���X�g�̍쐬�Ɏ��s\n");
		return false;
	}

	return true;
}

/// @brief �I��
void CGraphicsCommandWrapper::Finalize()
{
	Release();
}

/// @brief ���Z�b�g
void CGraphicsCommandWrapper::Reset()
{
	if (m_commandAllocator == nullptr)
	{
		printf("CGraphicsCommandWrapper::Reset m_commandAllocator��null�ł��B\n");
		return;
	}

	if (m_commandList == nullptr)
	{
		printf("CGraphicsCommandWrapper::Reset m_commandList��null�ł��B\n");
		return;
	}

	// �R�}���h�A���P�[�^�����Z�b�g
	HRESULT result = m_commandAllocator->Reset();
	assert(SUCCEEDED(result));

	// �R�}���h���X�g�����Z�b�g
	result = m_commandList->Reset(m_commandAllocator, nullptr);
	assert(SUCCEEDED(result));
}

/// @brief ���s
void CGraphicsCommandWrapper::Execute()
{
	if (m_commandList == nullptr)
	{
		printf("CGraphicsCommandWrapper::Execute m_commandList��null�ł��B\n");
		return;
	}

	if (m_commandQueue == nullptr)
	{
		printf("CGraphicsCommandWrapper::Execute m_commandQueue��null�ł��B\n");
		return;
	}

	// �R�}���h���X�g���N���[�Y����
	m_commandList->Close();

	// �R�}���h���X�g�����s
	ID3D12CommandList* commandLists[] = { m_commandList };
	m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
}

/// @brief ���\�[�X�o���A
/// @param _resource �o���A�����s���郊�\�[�X
/// @param _prev �O���
/// @param _next �����
void CGraphicsCommandWrapper::ResourceBarrier(ID3D12Resource* _resource, D3D12_RESOURCE_STATES _prev, D3D12_RESOURCE_STATES _next)
{
	if (_resource == nullptr)
	{
		printf("CGraphicsCommandWrapper::ResourceBarrier _resource��null�ł�\n");
		return;
	}

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;		//�T�u���\�[�X�̑J��
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	{
		D3D12_RESOURCE_TRANSITION_BARRIER transition;
		transition.pResource = _resource;
		transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		transition.StateBefore = _prev;		//�J�ڑO�̏��
		transition.StateAfter = _next;		//�J�ڌ�̏��
		barrier.Transition = transition;
	}
	m_commandList->ResourceBarrier(1, &barrier);
}

/// @brief �R�}���h�L���[���擾
/// @return �R�}���h�L���[
ID3D12CommandQueue* CGraphicsCommandWrapper::GetCommandQueue()
{
	return m_commandQueue;
}

/// @brief �R�}���h�A���P�[�^���擾
/// @return �R�}���h�A���P�[�^
ID3D12CommandAllocator* CGraphicsCommandWrapper::GetCommandAllocator()
{
	return m_commandAllocator;
}

/// @brief �R�}���h���X�g���擾
/// @return �R�}���h���X�g
ID3D12GraphicsCommandList* CGraphicsCommandWrapper::GetCommandList()
{
	return m_commandList;
}

/// @brief �J��
void CGraphicsCommandWrapper::Release()
{
	// �R�}���h���X�g���폜
	if (m_commandList != nullptr)
	{
		m_commandList->Release();
		m_commandList = nullptr;
	}

	// �R�}���h�A���P�[�^���폜
	if (m_commandAllocator != nullptr)
	{
		m_commandAllocator->Release();
		m_commandAllocator = nullptr;
	}

	// �R�}���h�L���[���폜
	if (m_commandQueue != nullptr)
	{
		m_commandQueue->Release();
		m_commandQueue = nullptr;
	}
}


/// @brief �R�}���h�L���[���쐬
/// @param _d3dDevice d3d�f�o�C�X
/// @return ����
bool CGraphicsCommandWrapper::CreateD3d12CommandQueue(ID3D12Device* _d3dDevice)
{
	if (m_commandQueue != nullptr)
	{
		printf("CGraphicsCommandWrapper::CreateD3d12CommandQueue �R�}���h�L���[�͐����ς݂ł�\n");
		return true;
	}

	if (_d3dDevice == nullptr)
	{
		printf("CGraphicsCommandWrapper::CreateD3d12CommandQueue _d3dDevice��null�ł�\n");
		return false;
	}

	//�R�}���h�L���[���쐬
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;					//�_�C���N�g�R�}���h�L���[
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;		//�ʏ�̗D��x
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;					//GPU�^�C���A�E�g���L��
	desc.NodeMask = 0;											//GPU��1�̂�

	HRESULT result = _d3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue));
	if (result != S_OK || m_commandQueue == nullptr)
	{
		printf("CGraphicsCommandWrapper::CreateD3d12CommandQueue D3D12�R�}���h�L���[�̐����Ɏ��s\n");
		return false;
	}

	return true;
}

/// @brief �R�}���h�A���P�[�^���쐬
/// @param _d3dDevice d3d�f�o�C�X
/// @return ����
bool CGraphicsCommandWrapper::CreateD3d12CommandAllocator(ID3D12Device* _d3dDevice)
{
	if (m_commandAllocator != nullptr)
	{
		printf("CGraphicsCommandWrapper::CreateD3d12CommandAllocator �R�}���h�A���P�[�^�[�͐����ς݂ł�\n");
		return true;
	}

	if (_d3dDevice == nullptr)
	{
		printf("CGraphicsCommandWrapper::CreateD3d12CommandAllocator _d3dDevice��null�ł�\n");
		return false;
	}

	HRESULT result = _d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));
	if (result != S_OK || m_commandAllocator == nullptr)
	{
		printf("CGraphicsCommandWrapper::CreateD3d12CommandAllocator �R�}���h�A���P�[�^�̐����Ɏ��s\n");
		return false;
	}

	return true;
}


/// @brief �R�}���h���X�g���쐬
/// @param _d3dDevice d3d�f�o�C�X
/// @return ����
bool CGraphicsCommandWrapper::CreateD3d12CommandList(ID3D12Device* _d3dDevice)
{
	if (m_commandList != nullptr)
	{
		printf("CGraphicsCommandWrapper::CreateD3d12CommandList �R�}���h���X�g�͐����ς݂ł�\n");
		return true;
	}

	if (m_commandAllocator == nullptr)
	{
		printf("CGraphicsCommandWrapper::CreateD3d12CommandList m_commandAllocator��null�ł�\n");
		return false;
	}

	if (_d3dDevice == nullptr)
	{
		printf("CGraphicsCommandWrapper::CreateD3d12CommandList d3d�f�o�C�X��null�ł�\n");
		return false;
	}

	HRESULT result = _d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator, nullptr, IID_PPV_ARGS(&m_commandList));
	if (result != S_OK || m_commandList == nullptr)
	{
		printf("CGraphicsCommandWrapper::CreateD3d12CommandList �R�}���h���X�g�̐����Ɏ��s\n");
		return false;
	}

	//�R�}���h���X�g���N���[�Y���Ă���
	m_commandList->Close();

	return true;
}
