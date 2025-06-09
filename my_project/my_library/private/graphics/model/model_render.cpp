#include "graphics/model/model_render.h"

CModelRender* CModelRender::s_instance = nullptr;

CModelRender::CModelRender()
{
}

CModelRender::~CModelRender()
{
}

/// @brief ������
void CModelRender::Initialize()
{

}

/// @brief �I��
void CModelRender::Finalize()
{
	for (auto& model : m_modelList)
	{
		if (model != nullptr)
		{
			model->Finalize();
			delete(model);
			model = nullptr;
		}
	}
	m_modelList.clear();
}

/// @brief �X�V
void CModelRender::Update()
{
	for (auto& model : m_modelList)
	{
		if (model != nullptr)
		{
			model->Update();
		}
	}
}

/// @brief �`��
void CModelRender::Draw()
{
	for (auto& model : m_modelList)
	{
		if (model != nullptr)
		{
			model->Draw();
		}
	}
}

/// @brief ��X�V
void CModelRender::PostUpdate()
{
	for (auto& model : m_modelList)
	{
		if (model != nullptr)
		{
			model->PostUpdate();
		}
	}
}

/// @brief ���f���쐬
/// @param _modelInitData ���f���������f�[�^
/// @return ���f��
CModel* CModelRender::CreateModel(const CModel::ModelInitData& _modelInitData)
{
	CModel* model = new(CModel);
	if (!model->Initialize(_modelInitData))
	{
		printf("CModelRender::AddModel ���f���̏������Ɏ��s\n");
		delete(model);
		return nullptr;
	}

	m_modelList.push_back(model);

	return model;
}

/// @brief �V���O���g���̃C���X�^���X����
void CModelRender::CreateInstance()
{
	if (!s_instance)
	{
		s_instance = new CModelRender();
	}
}

/// @brief �V���O���g���̃C���X�^���X�擾
/// @return GraphicsEngine�̃C���X�^���X�̎Q��
CModelRender& CModelRender::GetInstance()
{
	if (!s_instance)
	{
		throw;
	}
	return *s_instance;
}

/// @brief �V���O���g���̃C���X�^���X�폜
void CModelRender::Delete()
{
	if (s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}