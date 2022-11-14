#include "graphics/gui_window.h"
#include <Windows.h>

MY_LIB_NAMESPACE_BEGIN

LRESULT CALLBACK WndProc(HWND _hwnd, UINT _message, WPARAM _wParam, LPARAM _lParam)
{
	switch (_message)
	{
	case WM_DESTROY:
		//�E�B���h�E���폜���ꂽ�Ƃ�
		PostQuitMessage(0);
		break;

	default:
		break;
	}

	return DefWindowProc(_hwnd, _message, _wParam, _lParam);
}

/// @brief �R���X�g���N�^
CGUIWindow::CGUIWindow()
{

}

/// @brief �f�X�g���N�^
CGUIWindow::~CGUIWindow()
{
	Release();
}

/// @brief �������
void CGUIWindow::Release()
{

}

/// @brief ����������
/// @param _initData �������f�[�^
/// @return �������ɐ����������ǂ���(�������Ă����true)
bool CGUIWindow::Initialize(const InitData& _initData)
{
	WNDCLASS wc;
	HWND hwnd;

	HINSTANCE hInstance = static_cast<HINSTANCE>(_initData.hInstance);

	//�E�B���h�E�N���X�̐ݒ�
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

	//�E�B���h�E�N���X��o�^
	if (!RegisterClass(&wc))
	{
		return false;
	}

	//�E�B���h�E���쐬
	hwnd = CreateWindow
	(
		_initData.appName.c_str(),
		_initData.appName.c_str(),				//�E�B���h�E�̖��O
		WS_OVERLAPPEDWINDOW,
		0, 0,									//�E�B���h�E�̍��W
		_initData.width, _initData.height,		//�E�B���h�E�̃T�C�Y
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if (!hwnd)
	{
		//�E�B���h�E�̍쐬�Ɏ��s
		return false;
	}

	//�E�B���h�E��\��
	ShowWindow(hwnd, SW_SHOWNORMAL);

	//�E�B���h�E���ĕ`��
	UpdateWindow(hwnd);

	return true;
}

/// @brief �I������
void CGUIWindow::Finalize()
{

}

MY_LIB_NAMESPACE_END