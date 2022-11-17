#pragma once

//インクルード
//コピー禁止
#include "../noncopyable.h"
//標準ライブラリ
#include <string>

MY_LIB_NAMESPACE_BEGIN

//GUIWindow
class CGUIWindow : private Noncopyable
{
public:
	//初期化データ
	struct InitData
	{
		void* hInstance = nullptr;			//インスタンスハンドル
		const char* appName = nullptr;		//ウィンドウの名前
		unsigned int width = 0;				//ウィンドウの横幅
		unsigned int height = 0;			//ウィンドウの縦幅
	};

private:
	/// @brief コンストラクタ
	CGUIWindow();

public:
	/// @brief デストラクタ
	~CGUIWindow();

public:
	/// @brief 初期化処理
	/// @param _initData 初期化データ
	/// @return 初期化に成功したかどうか(成功していればtrue)
	bool Initialize(const InitData& _initData);

	/// @brief 終了処理
	void Finalize();

private:
	static CGUIWindow* s_instance;		//シングルトンのインスタンス

public:
	/// @brief シングルトンのインスタンス生成
	static void CreateInstance();

	/// @brief CGUIWindowのインスタンス取得
	/// @return CGUIWindowのインスタンスの参照
	static CGUIWindow& GetInstance();

	/// @brief シングルトンのインスタンス削除
	static void Delete();
};

MY_LIB_NAMESPACE_END