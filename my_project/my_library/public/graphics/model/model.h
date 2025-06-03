#pragma once
#include "shader/shader.h"
#include "graphics/model/model_def.h"
#include <d3d12.h>

MY_LIB_NAMESPACE_BEGIN

class CModel
{
public:
	struct VertexData
	{
		const float* vertexDataPtr = nullptr;
		unsigned long long vertexDataSize = 0;
		unsigned long long vertexDataStride = 0;
	};

	struct ModelInitData
	{
		VertexData vertexData;
		CShader::ShaderInitData vsInitData;
		CShader::ShaderInitData psInitData;
		MODEL_INPUT_LAYOUT_TYPE inputLayoutType = MODEL_INPUT_LAYOUT_TYPE_NONE;
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

	/// @brief ���[�g�V�O�l�`�����쐬
	/// @return ����
	bool CreateRootSignature();

	/// @brief �f�B�X�N���v�^�q�[�v���쐬
	/// @return ����
	bool CreateDescriptorHeap();

private:
	ID3D12RootSignature* m_rootSignature = nullptr;			// ���[�g�V�O�l�`��
	ID3D12DescriptorHeap* m_descriptorHeap = nullptr;		// �f�B�X�N���v�^�q�[�v
	ID3D12PipelineState* m_pipelineState = nullptr;			// �p�C�v���C���X�e�[�g
	ID3D12Resource* m_vertexBuffer = nullptr;				// ���_�o�b�t�@
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;			// ���_�o�b�t�@�r���[
	CShader m_vertexShader;		// ���_�V�F�[�_�[
	CShader m_pixelShader;		// �s�N�Z���V�F�[�_�[
};

MY_LIB_NAMESPACE_END
