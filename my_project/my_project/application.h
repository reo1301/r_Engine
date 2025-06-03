#pragma once

//インクルード
//コピー禁止
#include "../noncopyable.h"

GAME_NAMESPACE_BEGIN

//CApplication
class CApplication : private Noncopyable
{
private:
	/// @brief コンストラクタ
	CApplication();

public:
	/// @brief デストラクタ
	~CApplication();

public:
	/// @brief 初期化処理
	/// @param _windowWidth ウィンドウの横幅
	/// @param _windowHeight ウィンドウの縦幅
	void Initialize(unsigned int _windowWidth, unsigned int _windowHeight);

	/// @brief 終了処理
	void Finalize();

	/// @brief 前更新
	void PreUpdate();

	/// @brief 更新処理
	void Update();

	/// @brief 描画
	void Draw();
	
	/// @brief 後更新
	void PostUpdate();

private:
	static CApplication* s_instance;		//シングルトンのインスタンス

public:
	/// @brief シングルトンイのインスタンスの生成
	static void CreateInstance();

	/// @brief CApplicationのインスタンス取得
	/// @return CApplicationのインスタンスの参照
	static CApplication& GetInstance();

	/// @brief シングルトンのインスタンス削除
	static void Delete();
};

GAME_NAMESPACE_END