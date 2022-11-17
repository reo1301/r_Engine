#pragma once

//�C���N���[�h
//�R�s�[�֎~
#include "noncopyable.h"

MY_LIB_NAMESPACE_BEGIN

class CGraphicsEngine : private Noncopyable
{
private:
	/// @brief �R���X�g���N�^
	CGraphicsEngine();

public:
	/// @brief �f�X�g���N�^
	~CGraphicsEngine();

private:
	static CGraphicsEngine* s_instance;

public:
	/// @brief �V���O���g���̃C���X�^���X����
	static void CreateInstance();

	/// @brief �V���O���g���̃C���X�^���X�擾
	/// @return GraphicsEngine�̃C���X�^���X�̎Q��
	static CGraphicsEngine& GetInstance();

	/// @brief �V���O���g���̃C���X�^���X�폜
	static void Delete();
};

MY_LIB_NAMESPACE_END