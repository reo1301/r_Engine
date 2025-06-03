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

	/// @brief 初期化
	void Initialize();

	/// @brief 終了
	void Finalize();

	/// @brief 更新
	void Update();

	/// @brief 描画
	void Draw();

	/// @brief 後更新
	void PostUpdate();

	/// @brief モデル作成
	/// @param _modelInitData モデル初期化データ
	/// @return モデル
	CModel* CreateModel(const CModel::ModelInitData& _modelInitData);

private:
	static CModelRender* s_instance;		//シングルトンのインスタンス

public:
	/// @brief シングルトンのインスタンス生成
	static void CreateInstance();

	/// @brief シングルトンのインスタンス取得
	/// @return CModelRenderのインスタンスの参照
	static CModelRender& GetInstance();

	/// @brief シングルトンのインスタンス削除
	static void Delete();

private:
	std::list<CModel*> m_modelList;
};

MY_LIB_NAMESPACE_END
