#include "application.h"
#include <cstdlib>

GAME_NAMESPACE_BEGIN

//������
CApplication* CApplication::m_instance = nullptr;

/// @brief �R���X�g���N�^
CApplication::CApplication()
{

}

/// @brief �f�X�g���N�^
CApplication::~CApplication()
{
	Release();
}

/// @brief �������
void CApplication::Release()
{

}

/// @brief ����������
void CApplication::Initialize()
{

}

/// @brief �I������
void CApplication::Finalize()
{

}

/// @brief �X�V����
void CApplication::Update()
{

}

/// @brief �V���O���g���C�̃C���X�^���X�̐���
void CApplication::CreateInstance()
{
	//�C���X�^���X���܂��Ȃ��Ƃ�����
	if (!m_instance)
	{
		m_instance = new CApplication();
	}
}

/// @brief CApplication�̃C���X�^���X�擾
/// @return CApplication�̃C���X�^���X�̎Q��
CApplication& CApplication::GetInstance()
{
	if (!m_instance)
	{
		throw;
	}
	return *m_instance;
}

/// @brief �V���O���g���̃C���X�^���X�폜
void CApplication::Delete()
{
	if (m_instance)
	{
		delete m_instance;
		m_instance = nullptr;
	}
}

GAME_NAMESPACE_END