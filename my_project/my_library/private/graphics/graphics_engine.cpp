#include "graphics/graphics_engine.h"

//�C���N���[�h
//D3D�n
#include <d3d12.h>

//�W�����C�u�����n
#include <iostream>

MY_LIB_NAMESPACE_BEGIN

CGraphicsEngine* CGraphicsEngine::s_instance = nullptr;

/// @brief �R���X�g���N�^
CGraphicsEngine::CGraphicsEngine()
{

}

/// @brief �f�X�g���N�^
CGraphicsEngine::~CGraphicsEngine()
{

}

/// @brief ����������
void CGraphicsEngine::Initialize()
{

}

/// @brief �I������
void CGraphicsEngine::Finalize()
{

}

#ifdef _DEBUG

/// @brief �f�o�b�O���C���[�̏�����
void CGraphicsEngine::InitDebugLayer()
{
	ID3D12Debug* debugController = nullptr;

	//�f�o�b�O�R���g���[���[�̐���
	D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));

	if (debugController)
	{
		//�f�o�b�O�R���g���[���[�̗L����
		debugController->EnableDebugLayer();

		debugController->Release();
	}
	else
	{
		printf("�G���[ : CGraphicsEngine::InitDebugLayer(),�f�o�b�O�R���g���[���[�̐����Ɏ��s���܂���\n");
	}
}

#endif

/// @brief �V���O���g���̃C���X�^���X����
void CGraphicsEngine::CreateInstance()
{
	if (!s_instance)
	{
		s_instance = new CGraphicsEngine();
	}
}

/// @brief �V���O���g���̃C���X�^���X�擾
/// @return GraphicsEngine�̃C���X�^���X�̎Q��
CGraphicsEngine& CGraphicsEngine::GetInstance()
{
	if (!s_instance)
	{
		throw;
	}
	return *s_instance;
}

/// @brief �V���O���g���̃C���X�^���X�폜
void CGraphicsEngine::Delete()
{
	if (s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

MY_LIB_NAMESPACE_END