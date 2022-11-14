#include "graphics/gui_window.h"
#include <Windows.h>
#include "application.h"

using namespace MY_LIB_NAMESPACE;
using namespace GAME_NAMESPACE;

/// @brief ����������
void Initialize();
/// @brief �I������
void Finalize();
/// @brief �X�V����
void Update();

int WINAPI WinMain(_In_ HINSTANCE _hInstance, _In_opt_  HINSTANCE /*_hPrevInstance*/, _In_ LPSTR /*_lpCmdLine*/, _In_ int /*_nShowCmd*/)
{
	//�E�B���h�E�̏�����
	CGUIWindow guiWindow;
	{
		CGUIWindow::InitData initData;
		initData.hInstance = _hInstance;
		initData.appName = "������ǂ�";
		initData.width = 800;
		initData.height = 600;
		guiWindow.Initialize(initData);
	}

	//����������
	Initialize();

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

	guiWindow.Finalize();

	return 0;
}

/// @brief ����������
void Initialize()
{
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