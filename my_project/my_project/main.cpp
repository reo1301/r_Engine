//インクルード
//標準ライブラリ
#include <Windows.h>

//ウィンドウ系
#include "window/gui_window.h"

//ゲーム系
#include "application.h"

#ifdef _DEBUG
//ウィンドウ系
#include "window/console_window.h"
#endif

#define WINDOW_WIDTH		1280		//解像度の横幅
#define WINDOW_HEIGHT		720			//解像度の縦幅

/// @brief 初期化処理
void Initialize(const HINSTANCE _hInstance, const int _showCmd);
/// @brief 終了処理
void Finalize();
/// @brief 更新処理
void Update();

int WINAPI WinMain(_In_ HINSTANCE _hInstance, _In_opt_  HINSTANCE /*_hPrevInstance*/, _In_ LPSTR /*_lpCmdLine*/, _In_ int _nShowCmd)
{
	//初期化処理
	Initialize(_hInstance, _nShowCmd);

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
/// @param _showCmd ウィンドウの表示方法
void Initialize(const HINSTANCE _hInstance, const int _showCmd)
{
#ifdef _DEBUG
	{
		//コンソールウィンドウの初期化処理
		CConsoleWindow::CreateInstance();
		CConsoleWindow::GetInsntance().Initialize();
	}
#endif

	{
		//ウィンドウの初期化処理
		CGUIWindow::CreateInstance();
		CGUIWindow::InitData initData;
		initData.hInstance = _hInstance;
		initData.appName = "うぃんどう";
		initData.width = WINDOW_WIDTH;
		initData.height = WINDOW_HEIGHT;
		CGUIWindow::GetInstance().Initialize(initData, _showCmd);
	}

	{
		//アプリケーションクラスの初期化処理
		CApplication::CreateInstance();
		CApplication::GetInstance().Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);
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

#ifdef _DEBUG
	{
		//コンソールウィンドウの終了処理
		CConsoleWindow::GetInsntance().Finalize();
		CConsoleWindow::Delete();
	}
#endif
}

/// @brief 更新処理
void Update()
{
	{
		//アプリケーションクラスの更新処理
		CApplication& application = CApplication::GetInstance();
		application.PreUpdate();
		application.Update();
		application.Draw();
		application.PostUpdate();
	}
}