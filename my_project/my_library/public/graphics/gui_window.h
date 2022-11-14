#pragma once

//インクルード
#include <string>

MY_LIB_NAMESPACE_BEGIN

//GUIWindow
class CGUIWindow
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

public:
	/// @brief コンストラクタ
	CGUIWindow();

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
	/// @brief 解放処理
	void Release();

};

MY_LIB_NAMESPACE_END