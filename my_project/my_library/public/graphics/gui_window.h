#pragma once

//�C���N���[�h
#include <string>
#include "../my_lib_def.h"

MY_LIB_NAMESPACE_BEGIN

//GUIWindow
class CGUIWindow
{
public:
	//�������f�[�^
	struct InitData
	{
		void* hInstance = nullptr;		//�C���X�^���X�n���h��
		std::wstring appName;			//�E�B���h�E�̖��O
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