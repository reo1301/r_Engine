#include "window/console_window.h"

//�C���N���[�h
//�W�����C�u����
#include <Windows.h>


MY_LIB_NAMESPACE_BEGIN

CConsoleWindow* CConsoleWindow::s_instance = nullptr;

/// @brief �R���X�g���N�^
CConsoleWindow::CConsoleWindow()
{

}

/// @brief �f�X�g���N�^
CConsoleWindow::~CConsoleWindow()
{

}

/// @brief ����������
void CConsoleWindow::Initialize()
{
	//�R���\�[���E�B���h�E�𐶐�
	AllocConsole();
}

/// @brief �I������
void CConsoleWindow::Finalize()
{
	//�R���\�[���E�B���h�E��j��
	FreeConsole();
}

/// @brief �V���O���g���̃C���X�^���X�𐶐�
void CConsoleWindow::CreateInstance()
{
	if (!s_instance)
	{
		s_instance = new CConsoleWindow();
	}
}

/// @brief CConsoleWindow�̃C���X�^���X�擾
/// @return CConsoleWindow�̃C���X�^���X�̎Q��
CConsoleWindow& CConsoleWindow::GetInsntance()
{
	if (!s_instance)
	{
		throw;
	}
	return *s_instance;
}

/// @brief �V���O���g���̃C���X�^���X�폜
void CConsoleWindow::Delete()
{
	if (!s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

MY_LIB_NAMESPACE_END