#include "graphics/model/model.h"

#include "graphics/model/model_utility.h"
#include "graphics/graphics_engine.h"

CModel::CModel()
{
}

CModel::~CModel()
{
}

/// @brief �J��
void CModel::Release()
{
	m_vertexShader.Release();
	m_pixelShader.Release();

	if (m_rootSignature != nullptr)
	{
		m_rootSignature->Release();
		m_rootSignature = nullptr;
	}

	if (m_descriptorHeap != nullptr)
	{
		m_descriptorHeap->Release();
		m_descriptorHeap = nullptr;
	}

	if (m_pipelineState != nullptr)
	{
		m_pipelineState->Release();
		m_pipelineState = nullptr;
	}

	if (m_vertexBuffer != nullptr)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
}

/// @brief ������
/// @param _initData �������f�[�^
/// @return ����
bool CModel::Initialize(const ModelInitData& _initData)
{
	const VertexData& vertexData = _initData.vertexData;
	if ((vertexData.vertexDataPtr == nullptr) || (vertexData.vertexDataSize == 0) || (vertexData.vertexDataStride == 0))
	{
		printf("CModel::Initialize ���_�f�[�^������\n");
		return false;
	}

	if ((_initData.inputLayoutType == MODEL_INPUT_LAYOUT_TYPE_NONE) || (_initData.inputLayoutType >= MODEL_INPUT_LAYOUT_TYPE_NUM))
	{
		printf("CModel::Initialize inputLayoutType�������ł�\n");
		return false;
	}

	// todo �V�F�[�_�[�͖�����̂ł͂Ȃ��쐬�ς݂̂��̂�n�����A�ǂ����Ŏ��W���Ă����Č�������`�ɂ��������悳����
	// �V�F�[�_�[�̃��[�h
	if (!LoadShader(_initData.vsInitData, _initData.psInitData))
	{
		printf("CModel::Initialize �V�F�[�_�[�̃��[�h�Ɏ��s\n");
		return false;
	}

	ID3DBlob* vsBlob = m_vertexShader.GetShader();
	ID3DBlob* psBlob = m_pixelShader.GetShader();
	if (vsBlob == nullptr || psBlob == nullptr)
	{
		printf("CModel::Initialize �V�F�[�_�[�̎擾�Ɏ��s\n");
		return false;
	}

	// ���[�g�V�O�l�`���[�쐬
	if (!CreateRootSignature())
	{
		printf("CModel::Initialize ���[�g�V�O�l�`���̍쐬�Ɏ��s\n");
		return false;
	}

	// �p�C�v���C���X�e�[�g�쐬
	// ���̓��C�A�E�g���擾
	const MODEL_INPUT_LAYOUT_ELEMENT* inputLayout = nullptr;
	unsigned int elementNum = 0;
	if (!CModelUtility::GetModelInputLayout(inputLayout, elementNum, _initData.inputLayoutType) || (inputLayout == nullptr))
	{
		printf("CModel::Initialize ���f���̓��̓��C�A�E�g�̎擾�Ɏ��s�BinputLayoutType = %u\n", _initData.inputLayoutType);
		return false;
	}
	D3D12_INPUT_ELEMENT_DESC* inputDesc = static_cast<D3D12_INPUT_ELEMENT_DESC*>(malloc(sizeof(D3D12_INPUT_ELEMENT_DESC) * elementNum));
	for (unsigned int elementIdx = 0; elementIdx < elementNum; elementIdx++)
	{
		inputDesc[elementIdx] = { inputLayout[elementIdx].SemanticName, 0, inputLayout[elementIdx].format, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	}

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
	graphicsPipelineStateDesc.InputLayout = { inputDesc, elementNum };
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.NodeMask = 0;

	CGraphicsEngine& graphicsEngine = CGraphicsEngine::GetInstance();
	ID3D12Device* d3dDevice = graphicsEngine.GetD3dDevice();
	if (d3dDevice == nullptr)
	{
		printf("CModel::Initialize D3DDevice�������ł��B\n");
		return false;
	}

	HRESULT result = d3dDevice->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&m_pipelineState));
	if (FAILED(result))
	{
		printf("CModel::Initialize �p�C�v���C���X�e�[�g�̍쐬�Ɏ��s�B\n");
		return false;
	}

	// �q�[�v�v���p�e�B
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.CreationNodeMask = 1;
	heapProp.VisibleNodeMask = 1;

	// ���\�[�X���
	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = vertexData.vertexDataSize;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	result = d3dDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_vertexBuffer));
	if (FAILED(result))
	{
		printf("CModel::Initialize ���_�o�b�t�@�̍쐬�Ɏ��s�B\n");
		return false;
	}

	// ���_�o�b�t�@�ɃR�s�[
	void* vertexPtr = nullptr;
	result = m_vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vertexPtr));
	if (FAILED(result))
	{
		printf("CModel::Initialize ���_�o�b�t�@�̃}�b�v�Ɏ��s\n");
		return false;
	}
	memcpy_s(vertexPtr, vertexData.vertexDataSize, vertexData.vertexDataPtr, vertexData.vertexDataSize);
	m_vertexBuffer->Unmap(0, nullptr);

	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = static_cast<UINT>(vertexData.vertexDataSize);
	m_vertexBufferView.StrideInBytes = static_cast<UINT>(vertexData.vertexDataStride);

	return true;
}

/// @brief �X�V
void CModel::Update()
{

}

/// @brief �`��
void CModel::Draw()
{
	CGraphicsEngine& graphicsEngine = CGraphicsEngine::GetInstance();
	ID3D12GraphicsCommandList* d3dGraphicsCommandList = graphicsEngine.GetD3dGraphicsCommandList();
	if (d3dGraphicsCommandList == nullptr)
	{
		printf("CModel::Draw d3dGraphicsCommandList�̎擾�Ɏ��s\n");
		return;
	}
	d3dGraphicsCommandList->SetGraphicsRootSignature(m_rootSignature);
	d3dGraphicsCommandList->SetPipelineState(m_pipelineState);

	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewList[] = { m_vertexBufferView };
	// �h���[�R�[�����R�}���h���X�g�ɐς�
	d3dGraphicsCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	d3dGraphicsCommandList->IASetVertexBuffers(0, _countof(vertexBufferViewList), vertexBufferViewList);
	d3dGraphicsCommandList->DrawInstanced(3, 1, 0, 0);
}

/// @brief ��X�V
void CModel::PostUpdate()
{

}

/// @brief �V�F�[�_�[�̃��[�h
/// @param _vsInitData ���_�V�F�[�_�[�������f�[�^
/// @param _psInitData �s�N�Z���V�F�[�_�[�������f�[�^
/// @return ����
bool CModel::LoadShader(const CShader::ShaderInitData& _vsInitData, const CShader::ShaderInitData& _psInitData)
{
	// ���_�V�F�[�_�[
	if (!m_vertexShader.Load(_vsInitData))
	{
		printf("CModel::Initialize ���_�V�F�[�_�[�̃��[�h�Ɏ��s\n");
		return false;
	}

	// �s�N�Z���V�F�[�_�[
	if (!m_pixelShader.Load(_psInitData))
	{
		printf("CModel::Initialize �s�N�Z���V�F�[�_�[�̃��[�h�Ɏ��s\n");
		return false;
	}

	return true;
}

/// @return ����
bool CModel::CreateRootSignature()
{
	ID3D12Device* d3dDevice = CGraphicsEngine::GetInstance().GetD3dDevice();
	if (d3dDevice == nullptr)
	{
		printf("CModel::CreateRootSignature d3d�f�o�C�X�̎擾�Ɏ��s\n");
		return false;
	}

	D3D12_DESCRIPTOR_RANGE descriptorRanges[1] = {};
	descriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;		//�萔�o�b�t�@�Ƃ��Ďg�p
	descriptorRanges[0].NumDescriptors = 1;									//�f�B�X�N���v�^��1��
	descriptorRanges[0].BaseShaderRegister = 0;								//�V�F�[�_�[���W�X�^�̊J�n�C���f�b�N�X��0
	descriptorRanges[0].RegisterSpace = 0;									//���W�X�^�̈�͎g�p���Ȃ��̂�0
	descriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable = {};
	descriptorTable.NumDescriptorRanges = 1;
	descriptorTable.pDescriptorRanges = descriptorRanges;

	D3D12_ROOT_PARAMETER rootParameters[1] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//�萔�o�b�t�@�Ƃ��Ďg�p
	rootParameters[0].DescriptorTable = descriptorTable;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		//���ׂẴV�F�[�_�[����A�N�Z�X�\

	//���[�g�V�O�l�`�����쐬
	D3D12_ROOT_SIGNATURE_DESC desc;
	desc.NumParameters = _countof(rootParameters);		//���[�g�p�����[�^��
	desc.pParameters = rootParameters;					//���[�g�p�����[�^�̃|�C���^
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
	result = d3dDevice->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
	if (result != S_OK || m_rootSignature == nullptr)
	{
		printf("CModel::CreateRootSignature ���[�g�V�O�l�`���̐����Ɏ��s\n");
		return false;
	}

	return true;
}

/// @brief �f�B�X�N���v�^�q�[�v���쐬
/// @return ����
bool CModel::CreateDescriptorHeap()
{
	return true;
}
