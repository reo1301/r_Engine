#pragma once

GAME_NAMESPACE_BEGIN

//CApplication
class CApplication
{
private:
	/// @brief �R���X�g���N�^
	CApplication();

public:
	/// @brief �f�X�g���N�^
	~CApplication();

public:
	/// @brief ����������
	void Initialize();

	/// @brief �I������
	void Finalize();

	/// @brief �X�V����
	void Update();

private:
	/// @brief �������
	void Release();

private:
	static CApplication* m_instance;		//�V���O���g���̃C���X�^���X

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