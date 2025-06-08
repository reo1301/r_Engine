#include "graphics/graphics_pipeline_state_wrapper.h"
#include "shader/shader.h"

MY_LIB_NAMESPACE_BEGIN

CGraphicsPipelineStateWrapper::CGraphicsPipelineStateWrapper()
{

}

CGraphicsPipelineStateWrapper::~CGraphicsPipelineStateWrapper()
{

}

/// @brief ������
/// @param _initData �������f�[�^
/// @return ����
bool CGraphicsPipelineStateWrapper::Initialize(const InitData& _initData)
{
	if (!CreateRootSignature(_initData))
	{
		printf("CGraphicsPipelineStateWrapper::Initialize ���[�g�V�O�l�`���̍쐬�Ɏ��s\n");
		return false;
	}

	if (!CreatePipelineState(_initData))
	{
		printf("CGraphicsPipelineStateWrapper::Initialize �p�C�v���C���X�e�[�g�̍쐬�Ɏ��s\n");
		return false;
	}

	return true;
}

/// @brief �I��
void CGraphicsPipelineStateWrapper::Finalize()
{
	Release();
}

/// @brief ���[�g�V�O�l�`���ƃp�C�v���C���X�e�[�g��ݒ肷��
/// @param _commandList �ݒ肷��R�}���h���X�g
/// @return ����
bool CGraphicsPipelineStateWrapper::SetRootSignatureAndPipelineState(ID3D12GraphicsCommandList* _commandList)
{
	if (_commandList == nullptr)
	{
		printf("CGraphicsPipelineStateWrapper::SetPipelineState _commandList��null�ł�\n");
		return false;
	}

	_commandList->SetGraphicsRootSignature(m_rootSignature);
	_commandList->SetPipelineState(m_pipelineState);

	return true;
}

/// @brief �J��
void CGraphicsPipelineStateWrapper::Release()
{
	if (m_pipelineState != nullptr)
	{
		m_pipelineState->Release();
		m_pipelineState = nullptr;
	}

	if (m_rootSignature != nullptr)
	{
		m_rootSignature->Release();
		m_rootSignature = nullptr;
	}
}

/// @brief ���[�g�V�O�l�`���쐬
/// @param _initData �������f�[�^
/// @return ����
bool CGraphicsPipelineStateWrapper::CreateRootSignature(const InitData& _initData)
{
	if (_initData.d3dDevice == nullptr)
	{
		printf("CGraphicsPipelineStateWrapper::CreateRootSignature d3d�f�o�C�X��null�ł�\n");
		return false;
	}

	// todo ��U���[�g�p�����[�^���͋�ō쐬�i�K�v�ɂȂ����ۂɍ쐬����j
	//���[�g�V�O�l�`�����쐬
	D3D12_ROOT_SIGNATURE_DESC desc;
	desc.NumParameters = 0;								//���[�g�p�����[�^��
	desc.pParameters = nullptr;							//���[�g�p�����[�^�̃|�C���^
	desc.NumStaticSamplers = 0;							//�T���v���[��
	desc.pStaticSamplers = nullptr;						//�T���v���[�̃|�C���^
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;		//�I�v�V����

	HRESULT result;
	//���[�g�V�O�l�`���̃V���A���C�Y
	ID3DBlob* pSignature = nullptr;
	result = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &pSignature, nullptr);
	if (result != S_OK || pSignature == nullptr)
	{
		printf("CModel::CreateRootSignature ���[�g�V�O�l�`���̃V���A���C�Y�Ɏ��s\n");
		return false;
	}

	//���[�g�V�O�l�`���̍쐬
	result = _initData.d3dDevice->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
	if (result != S_OK || m_rootSignature == nullptr)
	{
		printf("CModel::CreateRootSignature ���[�g�V�O�l�`���̐����Ɏ��s\n");
		return false;
	}

	return true;
}

/// @brief �p�C�v���C���X�e�[�g�쐬
/// @param _initData �������f�[�^
/// @return ����
bool CGraphicsPipelineStateWrapper::CreatePipelineState(const InitData& _initData)
{
	if (_initData.d3dDevice == nullptr)
	{
		printf("CGraphicsPipelineStateWrapper::CreatePipelineState d3d�f�o�C�X��null�ł�\n");
		return false;
	}

	if (_initData.vsShader == nullptr)
	{
		printf("CGraphicsPipelineStateWrapper::CreatePipelineState vsShader��null�ł�\n");
		return false;
	}

	if (_initData.psShader == nullptr)
	{
		printf("CGraphicsPipelineStateWrapper::CreatePipelineState psShader��null�ł�\n");
		return false;
	}

	ID3DBlob* vsBlob = _initData.vsShader->GetShader();
	ID3DBlob* psBlob = _initData.psShader->GetShader();
	if (vsBlob == nullptr || psBlob == nullptr)
	{
		printf("CGraphicsPipelineStateWrapper::CreatePipelineState �V�F�[�_�[�̎擾�Ɏ��s\n");
		return false;
	}

	// todo �ЂƂ܂�CModel�ł��Ă����ݒ�����̂܂܎����Ă��Ă��邪�A�K�v�ɂȂ�����type�I�Ȃ��̂�n���Đݒ��؂�ւ�����悤�ɂ���
	
	// �����_�[�^�[�Q�b�g�̃u�����h���
	D3D12_RENDER_TARGET_BLEND_DESC rtBlendDesc = {};
	rtBlendDesc.BlendEnable = false;
	rtBlendDesc.LogicOpEnable = false;
	rtBlendDesc.SrcBlend = D3D12_BLEND_ONE;
	rtBlendDesc.DestBlend = D3D12_BLEND_ZERO;
	rtBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	rtBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	rtBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	rtBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	rtBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	rtBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// �u�����h���
	D3D12_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0] = rtBlendDesc;

	// ���X�^���C�U�[
	D3D12_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	rasterizerDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	rasterizerDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = false;
	rasterizerDesc.ForcedSampleCount = 0;
	rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	// �f�v�X�X�e���V��
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	depthStencilDesc.StencilEnable = false;

	// �p�C�v���C���X�e�[�g
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc = {};
	graphicsPipelineStateDesc.pRootSignature = m_rootSignature;
	graphicsPipelineStateDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.SampleMask = UINT_MAX;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.InputLayout = { _initData.inputElementList, _initData.inputElementNum };
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.NodeMask = 0;

	HRESULT result = _initData.d3dDevice->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&m_pipelineState));
	if (FAILED(result))
	{
		printf("CGraphicsPipelineStateWrapper::CreatePipelineState �p�C�v���C���X�e�[�g�̍쐬�Ɏ��s�B\n");
		return false;
	}

	return true;
}

MY_LIB_NAMESPACE_END
