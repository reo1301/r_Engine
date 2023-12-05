#pragma once

//�C���N���[�h
//�R�s�[�֎~
#include "../noncopyable.h"
//�W�����C�u����
#include <string>
#include <Windows.h>

MY_LIB_NAMESPACE_BEGIN

//GUIWindow
class CGUIWindow : private Noncopyable
{
public:
	//�������f�[�^
	struct InitData
	{
		void* hInstance = nullptr;			//�C���X�^���X�n���h��
		const char* appName = nullptr;		//�E�B���h�E�̖��O
		unsigned int width = 0;				//�E�B���h�E�̉���
		unsigned int height = 0;			//�E�B���h�E�̏c��
	};

private:
	/// @brief �R���X�g���N�^
	CGUIWindow();

public:
	/// @brief �f�X�g���N�^
	~CGUIWindow();

public:
	/// @brief ����������
	/// @param _initData �������f�[�^
	/// @param _showCmd �\�����@
	/// @return �������ɐ����������ǂ���(�������Ă����true)
	bool Initialize(const InitData& _initData, const int _showCmd);

	/// @brief �I������
	void Finalize();

	/// @brief �E�B���h�E�n���h���擾
	/// @return �E�B���h�E�n���h��
	HWND GetGuiWindowHandle() const;

	/// @brief �E�B���h�E�̉������擾
	/// @return �E�B���h�E�̉���
	unsigned int GetWindowWidth() const;
	
	/// @brief �E�B���h�E�̏c�����擾
	/// @return �E�B���h�E�̏c��
	unsigned int GetWindowHeight() const;

private:
	static CGUIWindow* s_instance;		//�V���O���g���̃C���X�^���X

public:
	/// @brief �V���O���g���̃C���X�^���X����
	static void CreateInstance();

	/// @brief CGUIWindow�̃C���X�^���X�擾
	/// @return CGUIWindow�̃C���X�^���X�̎Q��
	static CGUIWindow& GetInstance();

	/// @brief �V���O���g���̃C���X�^���X�폜
	static void Delete();

private:
	HWND m_windowHandle = 0;				//�E�B���h�E�n���h��
	unsigned int m_windowWidth = 0;			//�E�B���h�E�̉���
	unsigned int m_windowHeight = 0;		//�E�B���h�E�̏c��
};

MY_LIB_NAMESPACE_END