#ifdef _DEBUG

#pragma once

//�C���N���[�h
//�R�s�[�֎~
#include "../noncopyable.h"

MY_LIB_NAMESPACE_BEGIN

class CConsoleWindow : private Noncopyable
{
private:
	/// @brief �R���X�g���N�^
	CConsoleWindow();

public:
	/// @brief �f�X�g���N�^
	~CConsoleWindow();

public:
	/// @brief ����������
	void Initialize();

	/// @brief �I������
	void Finalize();

private:
	static CConsoleWindow* s_instance;		//�V���O���g���̃C���X�^���X

public:
	/// @brief �V���O���g���̃C���X�^���X�𐶐�
	static void CreateInstance();

	/// @brief CConsoleWindow�̃C���X�^���X�擾
	/// @return CConsoleWindow�̃C���X�^���X�̎Q��
	static CConsoleWindow& GetInsntance();

	/// @brief �V���O���g���̃C���X�^���X�폜
	static void Delete();

};

MY_LIB_NAMESPACE_END

#endif