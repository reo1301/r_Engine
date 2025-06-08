#pragma once
#include "shader/shader.h"
#include "graphics/model/model_def.h"
#include "graphics/pipeline_state_wrapper.h"
#include "graphics/vertex_buffer.h"
#include <d3d12.h>

MY_LIB_NAMESPACE_BEGIN

class CModel
{
public:
	struct ModelInitData
	{
		CVertexBuffer::VertexData vertexData;		// ���_�f�[�^
		CShader::ShaderInitData vsInitData;			// ���_�V�F�[�_�[�������f�[�^
		CShader::ShaderInitData psInitData;			// �s�N�Z���V�F�[�_�[�������f�[�^
		MODEL_INPUT_LAYOUT_TYPE inputLayoutType = MODEL_INPUT_LAYOUT_TYPE_NONE;		// ���̓��C�A�E�g�^�C�v
	};

public:
	CModel();
	~CModel();

	/// @brief �J��
	void Release();

	/// @brief ������
	/// @param _initData �������f�[�^
	/// @return ����
	bool Initialize(const ModelInitData& _initData);

	/// @brief �X�V
	void Update();

	/// @brief �`��
	void Draw();

	/// @brief ��X�V
	void PostUpdate();

private:
	/// @brief �V�F�[�_�[�̃��[�h
	/// @param _vsInitData ���_�V�F�[�_�[�������f�[�^
	/// @param _psInitData �s�N�Z���V�F�[�_�[�������f�[�^
	/// @return ����
	bool LoadShader(const CShader::ShaderInitData& _vsInitData, const CShader::ShaderInitData& _psInitData);

private:
	CGraphicsPipelineStateWrapper m_pipelineState;		// �p�C�v���C���X�e�[�g
	CVertexBuffer m_vertexBuffer;						// ���_�o�b�t�@
	CShader m_vertexShader;		// ���_�V�F�[�_�[
	CShader m_pixelShader;		// �s�N�Z���V�F�[�_�[
};

MY_LIB_NAMESPACE_END
