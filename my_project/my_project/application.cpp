#include "application.h"
#include <cstdlib>

GAME_NAMESPACE_BEGIN

//初期化
CApplication* CApplication::m_instance = nullptr;

/// @brief コンストラクタ
CApplication::CApplication()
{

}

/// @brief デストラクタ
CApplication::~CApplication()
{
	Release();
}

/// @brief 解放処理
void CApplication::Release()
{

}

/// @brief 初期化処理
void CApplication::Initialize()
{

}

/// @brief 終了処理
void CApplication::Finalize()
{

}

/// @brief 更新処理
void CApplication::Update()
{

}

/// @brief シングルトンイのインスタンスの生成
void CApplication::CreateInstance()
{
	//インスタンスがまだないとき生成
	if (!m_instance)
	{
		m_instance = new CApplication();
	}
}

/// @brief CApplicationのインスタンス取得
/// @return CApplicationのインスタンスの参照
CApplication& CApplication::GetInstance()
{
	if (!m_instance)
	{
		throw;
	}
	return *m_instance;
}

/// @brief シングルトンのインスタンス削除
void CApplication::Delete()
{
	if (m_instance)
	{
		delete m_instance;
		m_instance = nullptr;
	}
}

GAME_NAMESPACE_END