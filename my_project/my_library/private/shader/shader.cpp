#include "shader/shader.h"

#include "system/file_stream/file_stream.h"
#include <d3dcompiler.h>

MY_LIB_NAMESPACE_BEGIN

#define VS_SHADER_MODEL_NAME "vs_5_0"		// ���_�V�F�[�_�[�̃V�F�[�_�[���f����
#define PS_SHADER_MODEL_NAME "ps_5_0"		// �s�N�Z���V�F�[�_�[�̃V�F�[�_�[���f����

CShader::CShader()
{
}

CShader::~CShader()
{
	Release();
}

/// @brief �J��
void CShader::Release()
{
	if (m_shader != nullptr)
	{
		m_shader->Release();
		m_shader = nullptr;
	}
}

/// @brief �V�F�[�_�[�̃��[�h
/// @param _initData �������f�[�^
/// @return ����
bool CShader::Load(const ShaderInitData& _initData)
{
	if (m_shader != nullptr)
	{
		printf("Shader::Load �V�F�[�_�[�����łɐݒ肳��Ă��܂�\n");
		return false;
	}

	if (_initData.filePath == nullptr)
	{
		printf("Shader::Load �t�@�C���p�X�������ł�\n");
		return false;
	}

	if (_initData.entryPoint == nullptr)
	{
		printf("Shader::Load �G���g���[�|�C���g�������ł�\n");
		return false;
	}

	if (_initData.type == ShaderType_None)
	{
		printf("Shader::Load �V�F�[�_�[�^�C�v�������ł�\n");
		return false;
	}

	// �V�F�[�_�[�t�@�C���ǂݍ���
	CFileStream::FileStreamInfo fileStreamInfo;
	fileStreamInfo.filePath = _initData.filePath;
	fileStreamInfo.type = CFileStream::FileStreamType_ReadBinary;

	CFileStream fileStream;
	fileStream.StreamFile(fileStreamInfo);

	if (fileStreamInfo.fileBuffer == nullptr)
	{
		printf("Shader::Load �V�F�[�_�[�t�@�C���̃��[�h�Ɏ��s�BfilePath = %s\n", _initData.filePath);
		return false;
	}

	if (fileStreamInfo.fileSize == 0)
	{
		printf("Shader::Load �V�F�[�_�[�t�@�C���̃T�C�Y��0�BfilePath = %s\n", _initData.filePath);
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

	// �V�F�[�_�[�R���p�C��
	HRESULT result = D3DCompile2(static_cast<const char*>(fileStreamInfo.fileBuffer), fileStreamInfo.fileSize,
		nullptr, nullptr, nullptr,
		_initData.entryPoint, shaderModelName, compileFlags,
		0, 0, nullptr, 0, &m_shader, &errorBlob);

	bool isSuccess = true;
	if (FAILED(result))
	{
		isSuccess = false;
		printf("Shader::Load �V�F�[�_�[�̃R���p�C���Ɏ��s\n");
		if (errorBlob != nullptr)
		{
			printf(static_cast<char*>(errorBlob->GetBufferPointer()));
			errorBlob->Release();
		}
	}

	// CFileStream���Ń��������m�ۂ���Ă���̂Ń������J��
	free(fileStreamInfo.fileBuffer);

	return isSuccess;
}

/// @brief �V�F�[�_�[�擾
/// @return �R���p�C���ς݃V�F�[�_�[
ID3DBlob* CShader::GetShader()
{
	return m_shader;
}

MY_LIB_NAMESPACE_END
