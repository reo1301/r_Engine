#include "graphics/gui_window.h"
#include <Windows.h>

using namespace MY_LIB_NAMESPACE;

int WINAPI WinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _lpCmdLine, int _nCmdShow)
{
	//�E�B���h�E�̐���
	CGUIWindow guiWindow;
	CGUIWindow::InitData initData;
	initData.hInstance = _hInstance;
	initData.appName = L"������ǂ�";

	guiWindow.Initialize(initData);
	
	while (true)
	{

	}

	guiWindow.Finalize();

	return 0;
}