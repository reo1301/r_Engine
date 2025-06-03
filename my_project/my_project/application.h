#pragma once

//�C���N���[�h
//�R�s�[�֎~
#include "../noncopyable.h"

GAME_NAMESPACE_BEGIN

//CApplication
class CApplication : private Noncopyable
{
private:
	/// @brief �R���X�g���N�^
	CApplication();

public:
	/// @brief �f�X�g���N�^
	~CApplication();

public:
	/// @brief ����������
	/// @param _windowWidth �E�B���h�E�̉���
	/// @param _windowHeight �E�B���h�E�̏c��
	void Initialize(unsigned int _windowWidth, unsigned int _windowHeight);

	/// @brief �I������
	void Finalize();

	/// @brief �O�X�V
	void PreUpdate();

	/// @brief �X�V����
	void Update();

	/// @brief �`��
	void Draw();
	
	/// @brief ��X�V
	void PostUpdate();

private:
	static CApplication* s_instance;		//�V���O���g���̃C���X�^���X

public:
	/// @brief �V���O���g���C�̃C���X�^���X�̐���
	static void CreateInstance();

	/// @brief CApplication�̃C���X�^���X�擾
	/// @return CApplication�̃C���X�^���X�̎Q��
	static CApplication& GetInstance();

	/// @brief �V���O���g���̃C���X�^���X�폜
	static void Delete();
};

GAME_NAMESPACE_END