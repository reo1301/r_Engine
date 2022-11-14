#include "graphics/gui_window.h"
#include <Windows.h>

MY_LIB_NAMESPACE_BEGIN

LRESULT CALLBACK WndProc(HWND _hwnd, UINT _message, WPARAM _wParam, LPARAM _lParam)
{
	switch (_message)
	{
	case WM_DESTROY:
		//ウィンドウが削除されたとき
		PostQuitMessage(0);
		break;

	default:
		break;
	}

	return DefWindowProc(_hwnd, _message, _wParam, _lParam);
}

/// @brief コンストラクタ
CGUIWindow::CGUIWindow()
{

}

/// @brief デストラクタ
CGUIWindow::~CGUIWindow()
{
	Release();
}

/// @brief 解放処理
void CGUIWindow::Release()
{

}

/// @brief 初期化処理
/// @param _initData 初期化データ
/// @return 初期化に成功したかどうか(成功していればtrue)
bool CGUIWindow::Initialize(const InitData& _initData)
{
	WNDCLASS wc;
	HWND hwnd;

	HINSTANCE hInstance = static_cast<HINSTANCE>(_initData.hInstance);

	//ウィンドウクラスの設定
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = nullptr;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = _initData.appName.c_str();

	//ウィンドウクラスを登録
	if (!RegisterClass(&wc))
	{
		return false;
	}

	//ウィンドウを作成
	hwnd = CreateWindow
	(
		_initData.appName.c_str(),
		_initData.appName.c_str(),				//ウィンドウの名前
		WS_OVERLAPPEDWINDOW,
		0, 0,									//ウィンドウの座標
		_initData.width, _initData.height,		//ウィンドウのサイズ
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if (!hwnd)
	{
		//ウィンドウの作成に失敗
		return false;
	}

	//ウィンドウを表示
	ShowWindow(hwnd, SW_SHOWNORMAL);

	//ウィンドウを再描画
	UpdateWindow(hwnd);

	return true;
}

/// @brief 終了処理
void CGUIWindow::Finalize()
{

}

MY_LIB_NAMESPACE_END