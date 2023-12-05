#include "public/window/gui_window.h"

MY_LIB_NAMESPACE_BEGIN

CGUIWindow* CGUIWindow::s_instance = nullptr;
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

}

/// @brief 初期化処理
/// @param _initData 初期化データ
/// @param _showCmd 表示方法
/// @return 初期化に成功したかどうか(成功していればtrue)
bool CGUIWindow::Initialize(const InitData& _initData, const int _showCmd)
{
	WNDCLASS wc;

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
	wc.lpszClassName = _initData.appName;

	//ウィンドウクラスを登録
	if (!RegisterClass(&wc))
	{
		return false;
	}

	//ウィンドウを作成
	m_windowHandle = CreateWindowEx
	(
		0,
		_initData.appName,
		_initData.appName,				//ウィンドウの名前
		WS_OVERLAPPEDWINDOW,
		0, 0,									//ウィンドウの座標
		_initData.width, _initData.height,		//ウィンドウのサイズ
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if (m_windowHandle == 0)
	{
		//ウィンドウの作成に失敗
		return false;
	}

	//縦幅と横幅を記録しておく
	m_windowWidth = _initData.width;
	m_windowHeight = _initData.height;

	//ウィンドウを表示
	ShowWindow(m_windowHandle, _showCmd);

	//ウィンドウを再描画
	UpdateWindow(m_windowHandle);

	return true;
}

/// @brief 終了処理
void CGUIWindow::Finalize()
{

}

/// @brief ウィンドウハンドル取得
HWND CGUIWindow::GetGuiWindowHandle() const
{
	return m_windowHandle;
}

/// @brief ウィンドウの横幅を取得
/// @return ウィンドウの横幅
unsigned int CGUIWindow::GetWindowWidth() const
{
	return m_windowWidth;
}

/// @brief ウィンドウの縦幅を取得
/// @return ウィンドウの縦幅
unsigned int CGUIWindow::GetWindowHeight() const
{
	return m_windowHeight;
}

/// @brief シングルトンのインスタンス生成
void CGUIWindow::CreateInstance()
{
	if (!s_instance)
	{
		s_instance = new CGUIWindow();
	}
}

/// @brief CGUIWindowのインスタンス取得
/// @return CGUIWindowのインスタンスの参照
CGUIWindow& CGUIWindow::GetInstance()
{
	if (!s_instance)
	{
		throw;
	}
	return *s_instance;
}

/// @brief シングルトンのインスタンス削除
void CGUIWindow::Delete()
{
	if (s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

MY_LIB_NAMESPACE_END