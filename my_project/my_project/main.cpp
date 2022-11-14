#include "graphics/gui_window.h"
#include <Windows.h>
#include "application.h"

using namespace MY_LIB_NAMESPACE;
using namespace GAME_NAMESPACE;

/// @brief 初期化処理
void Initialize();
/// @brief 終了処理
void Finalize();
/// @brief 更新処理
void Update();

int WINAPI WinMain(_In_ HINSTANCE _hInstance, _In_opt_  HINSTANCE /*_hPrevInstance*/, _In_ LPSTR /*_lpCmdLine*/, _In_ int /*_nShowCmd*/)
{
	//ウィンドウの初期化
	CGUIWindow guiWindow;
	{
		CGUIWindow::InitData initData;
		initData.hInstance = _hInstance;
		initData.appName = "うぃんどう";
		initData.width = 800;
		initData.height = 600;
		guiWindow.Initialize(initData);
	}

	//初期化処理
	Initialize();

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

	guiWindow.Finalize();

	return 0;
}

/// @brief 初期化処理
void Initialize()
{
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