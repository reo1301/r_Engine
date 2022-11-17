#pragma once

//インクルード
//コピー禁止
#include "noncopyable.h"

MY_LIB_NAMESPACE_BEGIN

class CGraphicsEngine : private Noncopyable
{
private:
	/// @brief コンストラクタ
	CGraphicsEngine();

public:
	/// @brief デストラクタ
	~CGraphicsEngine();

private:
	static CGraphicsEngine* s_instance;

public:
	/// @brief シングルトンのインスタンス生成
	static void CreateInstance();

	/// @brief シングルトンのインスタンス取得
	/// @return GraphicsEngineのインスタンスの参照
	static CGraphicsEngine& GetInstance();

	/// @brief シングルトンのインスタンス削除
	static void Delete();
};

MY_LIB_NAMESPACE_END