#include "application.h"

//�C���N���[�h
//�O���t�B�b�N�n
#include "graphics/graphics_engine.h"

GAME_NAMESPACE_BEGIN

//������
CApplication* CApplication::s_instance = nullptr;

/// @brief �R���X�g���N�^
CApplication::CApplication()
{

}

/// @brief �f�X�g���N�^
CApplication::~CApplication()
{

}

/// @brief ����������
void CApplication::Initialize()
{
	//�O���t�B�b�N�X�G���W��
	CGraphicsEngine::CreateInstance();
	CGraphicsEngine::GetInstance().Initialize();
}

/// @brief �I������
void CApplication::Finalize()
{
	//�O���t�B�b�N�X�G���W��
	CGraphicsEngine::GetInstance().Finalize();
	CGraphicsEngine::Delete();
}

/// @brief �X�V����
void CApplication::Update()
{
	CGraphicsEngine& graphicsEngine = CGraphicsEngine::GetInstance();
	graphicsEngine.Update();
}

/// @brief �V���O���g���C�̃C���X�^���X�̐���
void CApplication::CreateInstance()
{
	//�C���X�^���X���܂��Ȃ��Ƃ�����
	if (!s_instance)
	{
		s_instance = new CApplication();
	}
}

/// @brief CApplication�̃C���X�^���X�擾
/// @return CApplication�̃C���X�^���X�̎Q��
CApplication& CApplication::GetInstance()
{
	if (!s_instance)
	{
		throw;
	}
	return *s_instance;
}

/// @brief �V���O���g���̃C���X�^���X�폜
void CApplication::Delete()
{
	if (s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

GAME_NAMESPACE_END