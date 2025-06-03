#include "shader/shader.h"

#include "system/file_stream/file_stream.h"
#include <d3dcompiler.h>

MY_LIB_NAMESPACE_BEGIN

#define VS_SHADER_MODEL_NAME "vs_5_0"		// 頂点シェーダーのシェーダーモデル名
#define PS_SHADER_MODEL_NAME "ps_5_0"		// ピクセルシェーダーのシェーダーモデル名

CShader::CShader()
{
}

CShader::~CShader()
{
	Release();
}

/// @brief 開放
void CShader::Release()
{
	if (m_shader != nullptr)
	{
		m_shader->Release();
		m_shader = nullptr;
	}
}

/// @brief シェーダーのロード
/// @param _initData 初期化データ
/// @return 成否
bool CShader::Load(const ShaderInitData& _initData)
{
	if (m_shader != nullptr)
	{
		printf("Shader::Load シェーダーがすでに設定されています\n");
		return false;
	}

	if (_initData.filePath == nullptr)
	{
		printf("Shader::Load ファイルパスが無効です\n");
		return false;
	}

	if (_initData.entryPoint == nullptr)
	{
		printf("Shader::Load エントリーポイントが無効です\n");
		return false;
	}

	if (_initData.type == ShaderType_None)
	{
		printf("Shader::Load シェーダータイプが無効です\n");
		return false;
	}

	// シェーダーファイル読み込み
	CFileStream::FileStreamInfo fileStreamInfo;
	fileStreamInfo.filePath = _initData.filePath;
	fileStreamInfo.type = CFileStream::FileStreamType_ReadBinary;

	CFileStream fileStream;
	fileStream.StreamFile(fileStreamInfo);

	if (fileStreamInfo.fileBuffer == nullptr)
	{
		printf("Shader::Load シェーダーファイルのロードに失敗。filePath = %s\n", _initData.filePath);
		return false;
	}

	if (fileStreamInfo.fileSize == 0)
	{
		printf("Shader::Load シェーダーファイルのサイズが0。filePath = %s\n", _initData.filePath);
		if (fileStreamInfo.fileBuffer != nullptr)
		{
			free(fileStreamInfo.fileBuffer);
		}
		return false;
	}

	const char* shaderModelName = nullptr;
	if (_initData.type == ShaderType_Vertex)
	{
		shaderModelName = VS_SHADER_MODEL_NAME;
	}
	else if (_initData.type == ShaderType_Pixel)
	{
		shaderModelName = PS_SHADER_MODEL_NAME;
	}

#ifdef _DEBUG
	unsigned int compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	unsigned int compileFlags = 0;
#endif
	ID3DBlob* errorBlob = nullptr;

	// シェーダーコンパイル
	HRESULT result = D3DCompile2(static_cast<const char*>(fileStreamInfo.fileBuffer), fileStreamInfo.fileSize,
		nullptr, nullptr, nullptr,
		_initData.entryPoint, shaderModelName, compileFlags,
		0, 0, nullptr, 0, &m_shader, &errorBlob);

	bool isSuccess = true;
	if (FAILED(result))
	{
		isSuccess = false;
		printf("Shader::Load シェーダーのコンパイルに失敗\n");
		if (errorBlob != nullptr)
		{
			printf(static_cast<char*>(errorBlob->GetBufferPointer()));
			errorBlob->Release();
		}
	}

	// CFileStream内でメモリが確保されているのでメモリ開放
	free(fileStreamInfo.fileBuffer);

	return isSuccess;
}

/// @brief シェーダー取得
/// @return コンパイル済みシェーダー
ID3DBlob* CShader::GetShader()
{
	return m_shader;
}

MY_LIB_NAMESPACE_END
