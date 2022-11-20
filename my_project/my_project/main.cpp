//�C���N���[�h
//�W�����C�u����
#include <Windows.h>

//�E�B���h�E�n
#include "window/gui_window.h"

//�Q�[���n
#include "application.h"

#ifdef _DEBUG
//�E�B���h�E�n
#include "window/console_window.h"
#endif

/// @brief ����������
void Initialize(HINSTANCE _hInstance);
/// @brief �I������
void Finalize();
/// @brief �X�V����
void Update();

int WINAPI WinMain(_In_ HINSTANCE _hInstance, _In_opt_  HINSTANCE /*_hPrevInstance*/, _In_ LPSTR /*_lpCmdLine*/, _In_ int /*_nShowCmd*/)
{
	//����������
	Initialize(_hInstance);

	//���b�Z�[�W���[�v
	MSG msg = {};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//�X�V����
		Update();
	}

	//�I������
	Finalize();

	return 0;
}

/// @brief ����������
/// @param _hInstance �C���X�^���X�n���h��
void Initialize(HINSTANCE _hInstance)
{
#ifdef _DEBUG
	{
		//�R���\�[���E�B���h�E�̏���������
		CConsoleWindow::CreateInstance();
		CConsoleWindow::GetInsntance().Initialize();
	}
#endif

	{
		//�E�B���h�E�̏���������
		CGUIWindow::CreateInstance();
		CGUIWindow::InitData initData;
		initData.hInstance = _hInstance;
		initData.appName = "������ǂ�";
		initData.width = 800;
		initData.height = 600;
		CGUIWindow::GetInstance().Initialize(initData);
	}

	{
		//�A�v���P�[�V�����N���X�̏���������
		CApplication::CreateInstance();
		CApplication::GetInstance().Initialize();
	}
}

/// @brief �I������
void Finalize()
{
	{
		//�A�v���P�[�V�����N���X�̏I������
		CApplication::GetInstance().Finalize();
		CApplication::Delete();
	}

	{
		//�E�B���h�E�̏I������
		CGUIWindow::GetInstance().Finalize();
		CGUIWindow::Delete();
	}

#ifdef _DEBUG
	{
		//�R���\�[���E�B���h�E�̏I������
		CConsoleWindow::GetInsntance().Finalize();
		CConsoleWindow::Delete();
	}
#endif
}

/// @brief �X�V����
void Update()
{
	{
		//�A�v���P�[�V�����N���X�̍X�V����
		CApplication& application = CApplication::GetInstance();
		application.Update();
	}
}