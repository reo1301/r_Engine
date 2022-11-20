#pragma once

//�C���N���[�h
//�R�s�[�֎~
#include "../noncopyable.h"

MY_LIB_NAMESPACE_BEGIN

class CGraphicsEngine : private Noncopyable
{
private:
	/// @brief �R���X�g���N�^
	CGraphicsEngine();

public:
	/// @brief �f�X�g���N�^
	~CGraphicsEngine();

public:
	/// @brief ����������
	void Initialize();

	/// @brief �I������
	void Finalize();

private:
#ifdef _DEBUG

	/// @brief �f�o�b�O���C���[�̏�����
	void InitDebugLayer();

#endif

private:
	static CGraphicsEngine* s_instance;		//�V���O���g���̃C���X�^���X

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