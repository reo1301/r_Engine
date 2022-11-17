#pragma once

//�C���N���[�h
//�R�s�[�֎~
#include "../noncopyable.h"
//�W�����C�u����
#include <string>

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
	/// @return �������ɐ����������ǂ���(�������Ă����true)
	bool Initialize(const InitData& _initData);

	/// @brief �I������
	void Finalize();

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
};

MY_LIB_NAMESPACE_END