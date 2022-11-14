#include "graphics/gui_window.h"
#include <Windows.h>

using namespace MY_LIB_NAMESPACE;

int WINAPI WinMain(_In_ HINSTANCE _hInstance, _In_opt_  HINSTANCE /*_hPrevInstance*/, _In_ LPSTR /*_lpCmdLine*/, _In_ int /*_nShowCmd*/)
{
	//ウィンドウの生成
	CGUIWindow guiWindow;
	CGUIWindow::InitData initData;
	initData.hInstance = _hInstance;
	initData.appName = "うぃんどう";
	initData.width = 800;
	initData.height = 600;

	guiWindow.Initialize(initData);

	//メッセージループ
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	guiWindow.Finalize();

	return 0;
}