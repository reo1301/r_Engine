#pragma once
#include "../noncopyable.h"
#include "model.h"
#include <list>

MY_LIB_NAMESPACE_BEGIN

class CModelRender : private Noncopyable
{
private:
	CModelRender();

public:
	~CModelRender();

	/// @brief ������
	void Initialize();

	/// @brief �I��
	void Finalize();

	/// @brief �X�V
	void Update();

	/// @brief �`��
	void Draw();

	/// @brief ��X�V
	void PostUpdate();

	/// @brief ���f���쐬
	/// @param _modelInitData ���f���������f�[�^
	/// @return ���f��
	CModel* CreateModel(const CModel::ModelInitData& _modelInitData);

private:
	static CModelRender* s_instance;		//�V���O���g���̃C���X�^���X

public:
	/// @brief �V���O���g���̃C���X�^���X����
	static void CreateInstance();

	/// @brief �V���O���g���̃C���X�^���X�擾
	/// @return CModelRender�̃C���X�^���X�̎Q��
	static CModelRender& GetInstance();

	/// @brief �V���O���g���̃C���X�^���X�폜
	static void Delete();

private:
	std::list<CModel*> m_modelList;
};

MY_LIB_NAMESPACE_END
