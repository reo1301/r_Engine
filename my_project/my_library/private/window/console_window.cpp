#ifdef _DEBUG

#include "window/console_window.h"

//インクルード
//標準ライブラリ
#include <Windows.h>
#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <tchar.h>

MY_LIB_NAMESPACE_BEGIN

CConsoleWindow* CConsoleWindow::s_instance = nullptr;

/// @brief コンストラクタ
CConsoleWindow::CConsoleWindow()
{

}

/// @brief デストラクタ
CConsoleWindow::~CConsoleWindow()
{

}

/// @brief 初期化処理
void CConsoleWindow::Initialize()
{
	//コンソールウィンドウを生成
	AllocConsole();

	//書き込みモードで現在開いているコンソールウィンドウを開く(printfやcoutを使用できるようにする)
	//fileは結局コンソールウィンドウ閉じられたときに無効になるからローカルで問題ない?
	FILE* file = nullptr;
	freopen_s(&file, "CONOUT$", "w", stdout);
}

/// @brief 終了処理
void CConsoleWindow::Finalize()
{
	//コンソールウィンドウを破棄
	FreeConsole();

}

/// @brief シングルトンのインスタンスを生成
void CConsoleWindow::CreateInstance()
{
	if (!s_instance)
	{
		s_instance = new CConsoleWindow();
	}
}

/// @brief CConsoleWindowのインスタンス取得
/// @return CConsoleWindowのインスタンスの参照
CConsoleWindow& CConsoleWindow::GetInsntance()
{
	if (!s_instance)
	{
		throw;
	}
	return *s_instance;
}

/// @brief シングルトンのインスタンス削除
void CConsoleWindow::Delete()
{
	if (s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

MY_LIB_NAMESPACE_END

#endif