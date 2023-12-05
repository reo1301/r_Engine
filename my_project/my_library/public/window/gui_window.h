#pragma once

//インクルード
//コピー禁止
#include "../noncopyable.h"
//標準ライブラリ
#include <string>
#include <Windows.h>

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
	/// @param _showCmd 表示方法
	/// @return 初期化に成功したかどうか(成功していればtrue)
	bool Initialize(const InitData& _initData, const int _showCmd);

	/// @brief 終了処理
	void Finalize();

	/// @brief ウィンドウハンドル取得
	/// @return ウィンドウハンドル
	HWND GetGuiWindowHandle() const;

	/// @brief ウィンドウの横幅を取得
	/// @return ウィンドウの横幅
	unsigned int GetWindowWidth() const;
	
	/// @brief ウィンドウの縦幅を取得
	/// @return ウィンドウの縦幅
	unsigned int GetWindowHeight() const;

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

private:
	HWND m_windowHandle = 0;				//ウィンドウハンドル
	unsigned int m_windowWidth = 0;			//ウィンドウの横幅
	unsigned int m_windowHeight = 0;		//ウィンドウの縦幅
};

MY_LIB_NAMESPACE_END