#ifdef _DEBUG

#pragma once

//インクルード
//コピー禁止
#include "../noncopyable.h"

MY_LIB_NAMESPACE_BEGIN

class CConsoleWindow : private Noncopyable
{
private:
	/// @brief コンストラクタ
	CConsoleWindow();

public:
	/// @brief デストラクタ
	~CConsoleWindow();

public:
	/// @brief 初期化処理
	void Initialize();

	/// @brief 終了処理
	void Finalize();

private:
	static CConsoleWindow* s_instance;		//シングルトンのインスタンス

public:
	/// @brief シングルトンのインスタンスを生成
	static void CreateInstance();

	/// @brief CConsoleWindowのインスタンス取得
	/// @return CConsoleWindowのインスタンスの参照
	static CConsoleWindow& GetInsntance();

	/// @brief シングルトンのインスタンス削除
	static void Delete();

};

MY_LIB_NAMESPACE_END

#endif