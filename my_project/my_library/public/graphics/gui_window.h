#pragma once

//�C���N���[�h
#include <string>

MY_LIB_NAMESPACE_BEGIN

//GUIWindow
class CGUIWindow
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

public:
	/// @brief �R���X�g���N�^
	CGUIWindow();

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
	/// @brief �������
	void Release();

};

MY_LIB_NAMESPACE_END