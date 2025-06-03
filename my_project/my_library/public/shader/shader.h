#pragma once

#include <d3d12.h>

MY_LIB_NAMESPACE_BEGIN

class CShader
{
public:
	enum ShaderType
	{
		ShaderType_None = 0,		// ����
		ShaderType_Vertex,			// ���_�V�F�[�_�[
		ShaderType_Pixel,			// �s�N�Z���V�F�[�_�[

		ShaderType_Num
	};

	struct ShaderInitData
	{
		const char* filePath = nullptr;			// �t�@�C���p�X
		const char* entryPoint = nullptr;		// �G���g���[�|�C���g
		ShaderType type = ShaderType_None;		// �^�C�v
	};

public:
	CShader();
	~CShader();

	/// @brief �J��
	void Release();

	/// @brief �V�F�[�_�[�̃��[�h
	/// @param _initData �������f�[�^
	/// @return ����
	bool Load(const ShaderInitData& _initData);

	/// @brief �V�F�[�_�[�擾
	/// @return �R���p�C���ς݃V�F�[�_�[
	ID3DBlob* GetShader();

private:
	ID3DBlob* m_shader = nullptr;		// �R���p�C���ς݃V�F�[�_�[
};

MY_LIB_NAMESPACE_END
