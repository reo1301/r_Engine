#include "application.h"

//インクルード
//グラフィック系
#include "graphics/graphics_engine.h"

GAME_NAMESPACE_BEGIN

//初期化
CApplication* CApplication::s_instance = nullptr;

/// @brief コンストラクタ
CApplication::CApplication()
{

}

/// @brief デストラクタ
CApplication::~CApplication()
{

}

/// @brief 初期化処理
void CApplication::Initialize()
{
	//グラフィックスエンジン
	CGraphicsEngine::CreateInstance();
	CGraphicsEngine::GetInstance().Initialize();
}

/// @brief 終了処理
void CApplication::Finalize()
{
	//グラフィックスエンジン
	CGraphicsEngine::GetInstance().Finalize();
	CGraphicsEngine::Delete();
}

/// @brief 更新処理
void CApplication::Update()
{
	CGraphicsEngine& graphicsEngine = CGraphicsEngine::GetInstance();
	graphicsEngine.Update();
}

/// @brief シングルトンイのインスタンスの生成
void CApplication::CreateInstance()
{
	//インスタンスがまだないとき生成
	if (!s_instance)
	{
		s_instance = new CApplication();
	}
}

/// @brief CApplicationのインスタンス取得
/// @return CApplicationのインスタンスの参照
CApplication& CApplication::GetInstance()
{
	if (!s_instance)
	{
		throw;
	}
	return *s_instance;
}

/// @brief シングルトンのインスタンス削除
void CApplication::Delete()
{
	if (s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

GAME_NAMESPACE_END