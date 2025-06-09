#include "graphics/model/model_render.h"

CModelRender* CModelRender::s_instance = nullptr;

CModelRender::CModelRender()
{
}

CModelRender::~CModelRender()
{
}

/// @brief 初期化
void CModelRender::Initialize()
{

}

/// @brief 終了
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

/// @brief 更新
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

/// @brief 描画
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

/// @brief 後更新
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

/// @brief モデル作成
/// @param _modelInitData モデル初期化データ
/// @return モデル
CModel* CModelRender::CreateModel(const CModel::ModelInitData& _modelInitData)
{
	CModel* model = new(CModel);
	if (!model->Initialize(_modelInitData))
	{
		printf("CModelRender::AddModel モデルの初期化に失敗\n");
		delete(model);
		return nullptr;
	}

	m_modelList.push_back(model);

	return model;
}

/// @brief シングルトンのインスタンス生成
void CModelRender::CreateInstance()
{
	if (!s_instance)
	{
		s_instance = new CModelRender();
	}
}

/// @brief シングルトンのインスタンス取得
/// @return GraphicsEngineのインスタンスの参照
CModelRender& CModelRender::GetInstance()
{
	if (!s_instance)
	{
		throw;
	}
	return *s_instance;
}

/// @brief シングルトンのインスタンス削除
void CModelRender::Delete()
{
	if (s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}