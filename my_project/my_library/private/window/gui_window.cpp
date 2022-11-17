#include "public/window/gui_window.h"

//�C���N���[�h
//�W�����C�u����
#include <Windows.h>

MY_LIB_NAMESPACE_BEGIN

CGUIWindow* CGUIWindow::s_instance = nullptr;
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
	wc.lpszClassName = _initData.appName;

	//�E�B���h�E�N���X��o�^
	if (!RegisterClass(&wc))
	{
		return false;
	}

	//�E�B���h�E���쐬
	hwnd = CreateWindow
	(
		_initData.appName,
		_initData.appName,				//�E�B���h�E�̖��O
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

/// @brief �V���O���g���̃C���X�^���X����
void CGUIWindow::CreateInstance()
{
	if (!s_instance)
	{
		s_instance = new CGUIWindow();
	}
}

/// @brief CGUIWindow�̃C���X�^���X�擾
/// @return CGUIWindow�̃C���X�^���X�̎Q��
CGUIWindow& CGUIWindow::GetInstance()
{
	if (!s_instance)
	{
		throw;
	}
	return *s_instance;
}

/// @brief �V���O���g���̃C���X�^���X�폜
void CGUIWindow::Delete()
{
	if (!s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

MY_LIB_NAMESPACE_END