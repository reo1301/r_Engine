#pragma once
#include <d3d12.h>

MY_LIB_NAMESPACE_BEGIN

// �O���錾
class CShader;

class CGraphicsPipelineStateWrapper
{
public:
	struct InitData
	{
		ID3D12Device* d3dDevice = nullptr;							// d3d�f�o�C�X
		CShader* vsShader = nullptr;								// ���_�V�F�[�_�[
		CShader* psShader = nullptr;								// �s�N�Z���V�F�[�_�[
		D3D12_INPUT_ELEMENT_DESC* inputElementList = nullptr;		// ���̓��C�A�E�g
		unsigned int inputElementNum = 0;							// ���̓��C�A�E�g��
	};

public:
	CGraphicsPipelineStateWrapper();
	~CGraphicsPipelineStateWrapper();

	/// @brief ������
	/// @param _initData �������f�[�^
	/// @return ����
	bool Initialize(const InitData& _initData);

	/// @brief �I��
	void Finalize();

	/// @brief ���[�g�V�O�l�`���ƃp�C�v���C���X�e�[�g��ݒ肷��
	/// @param _commandList �ݒ肷��R�}���h���X�g
	/// @return ����
	bool SetRootSignatureAndPipelineState(ID3D12GraphicsCommandList* _commandList);

private:
	/// @brief �J��
	void Release();

	/// @brief ���[�g�V�O�l�`���쐬
	/// @param _initData �������f�[�^
	/// @return ����
	bool CreateRootSignature(const InitData& _initData);

	/// @brief �p�C�v���C���X�e�[�g�쐬
	/// @param _initData �������f�[�^
	/// @return ����
	bool CreatePipelineState(const InitData& _initData);

private:
	ID3D12RootSignature* m_rootSignature = nullptr;		// ���[�g�V�O�l�`��
	ID3D12PipelineState* m_pipelineState = nullptr;		// �p�C�v���C���X�e�[�g
};

MY_LIB_NAMESPACE_END
