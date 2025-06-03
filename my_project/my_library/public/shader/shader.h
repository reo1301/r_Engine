#pragma once

#include <d3d12.h>

MY_LIB_NAMESPACE_BEGIN

class CShader
{
public:
	enum ShaderType
	{
		ShaderType_None = 0,		// 無効
		ShaderType_Vertex,			// 頂点シェーダー
		ShaderType_Pixel,			// ピクセルシェーダー

		ShaderType_Num
	};

	struct ShaderInitData
	{
		const char* filePath = nullptr;			// ファイルパス
		const char* entryPoint = nullptr;		// エントリーポイント
		ShaderType type = ShaderType_None;		// タイプ
	};

public:
	CShader();
	~CShader();

	/// @brief 開放
	void Release();

	/// @brief シェーダーのロード
	/// @param _initData 初期化データ
	/// @return 成否
	bool Load(const ShaderInitData& _initData);

	/// @brief シェーダー取得
	/// @return コンパイル済みシェーダー
	ID3DBlob* GetShader();

private:
	ID3DBlob* m_shader = nullptr;		// コンパイル済みシェーダー
};

MY_LIB_NAMESPACE_END
