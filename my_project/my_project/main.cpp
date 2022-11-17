//インクルード
//標準ライブラリ
#include <Windows.h>

//ウィンドウ系
#include "window/gui_window.h"
#include "window/console_window.h"

//ゲーム系
#include "application.h"

using namespace MY_LIB_NAMESPACE;
using namespace GAME_NAMESPACE;

/// @brief 初期化処理
void Initialize(HINSTANCE _hInstance);
/// @brief 終了処理
void Finalize();
/// @brief 更新処理
void Update();

int WINAPI WinMain(_In_ HINSTANCE _hInstance, _In_opt_  HINSTANCE /*_hPrevInstance*/, _In_ LPSTR /*_lpCmdLine*/, _In_ int /*_nShowCmd*/)
{
	//初期化処理
	Initialize(_hInstance);

	//メッセージループ
	MSG msg = {};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//更新処理
		Update();
	}

	//終了処理
	Finalize();

	return 0;
}

/// @brief 初期化処理
/// @param _hInstance インスタンスハンドル
void Initialize(HINSTANCE _hInstance)
{
	{
		//コンソールウィンドウの初期化処理
		CConsoleWindow::CreateInstance();
		CConsoleWindow::GetInsntance().Initialize();
	}

	{
		//ウィンドウの初期化処理
		CGUIWindow::CreateInstance();
		CGUIWindow::InitData initData;
		initData.hInstance = _hInstance;
		initData.appName = "うぃんどう";
		initData.width = 800;
		initData.height = 600;
		CGUIWindow::GetInstance().Initialize(initData);
	}

	{
		//アプリケーションクラスの初期化処理
		CApplication::CreateInstance();
		CApplication::GetInstance().Initialize();
	}
}

/// @brief 終了処理
void Finalize()
{
	{
		//アプリケーションクラスの終了処理
		CApplication::GetInstance().Finalize();
		CApplication::Delete();
	}

	{
		//ウィンドウの終了処理
		CGUIWindow::GetInstance().Finalize();
		CGUIWindow::Delete();
	}

	{
		//コンソールウィンドウの終了処理
		CConsoleWindow::GetInsntance().Finalize();
		CConsoleWindow::Delete();
	}
}

/// @brief 更新処理
void Update()
{
	{
		//アプリケーションクラスの更新処理
		CApplication& application = CApplication::GetInstance();
		application.Update();
	}
}