#include "graphics/graphics_engine.h"

//インクルード
//D3D系
#include <d3d12.h>

//標準ライブラリ系
#include <iostream>

MY_LIB_NAMESPACE_BEGIN

CGraphicsEngine* CGraphicsEngine::s_instance = nullptr;

/// @brief コンストラクタ
CGraphicsEngine::CGraphicsEngine()
{

}

/// @brief デストラクタ
CGraphicsEngine::~CGraphicsEngine()
{

}

/// @brief 初期化処理
void CGraphicsEngine::Initialize()
{

}

/// @brief 終了処理
void CGraphicsEngine::Finalize()
{

}

#ifdef _DEBUG

/// @brief デバッグレイヤーの初期化
void CGraphicsEngine::InitDebugLayer()
{
	ID3D12Debug* debugController = nullptr;

	//デバッグコントローラーの生成
	D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));

	if (debugController)
	{
		//デバッグコントローラーの有効化
		debugController->EnableDebugLayer();

		debugController->Release();
	}
	else
	{
		printf("エラー : CGraphicsEngine::InitDebugLayer(),デバッグコントローラーの生成に失敗しました\n");
	}
}

#endif

/// @brief シングルトンのインスタンス生成
void CGraphicsEngine::CreateInstance()
{
	if (!s_instance)
	{
		s_instance = new CGraphicsEngine();
	}
}

/// @brief シングルトンのインスタンス取得
/// @return GraphicsEngineのインスタンスの参照
CGraphicsEngine& CGraphicsEngine::GetInstance()
{
	if (!s_instance)
	{
		throw;
	}
	return *s_instance;
}

/// @brief シングルトンのインスタンス削除
void CGraphicsEngine::Delete()
{
	if (s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

MY_LIB_NAMESPACE_END