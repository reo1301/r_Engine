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
	if (m_vertexBuffer != nullptr)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}

	m_pipelineState.Finalize();

	m_vertexShader.Release();
	m_pixelShader.Release();
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

	CGraphicsEngine& graphicsEngine = CGraphicsEngine::GetInstance();
	ID3D12Device* d3dDevice = graphicsEngine.GetD3dDevice();
	if (d3dDevice == nullptr)
	{
		printf("CModel::Initialize D3DDevice�������ł��B\n");
		return false;
	}

	// �p�C�v���C���X�e�[�g�쐬
	CGraphicsPipelineStateWrapper::InitData pipelineStateInitData;
	pipelineStateInitData.d3dDevice = d3dDevice;
	pipelineStateInitData.vsShader = &m_vertexShader;
	pipelineStateInitData.psShader = &m_pixelShader;
	pipelineStateInitData.inputElementList = inputDesc;
	pipelineStateInitData.inputElementNum = elementNum;
	m_pipelineState.Initialize(pipelineStateInitData);

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

	HRESULT result = d3dDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_vertexBuffer));
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

	if (!m_pipelineState.SetRootSignatureAndPipelineState(d3dGraphicsCommandList))
	{
		printf("CModel::Draw ���[�g�V�O�l�`���ƃp�C�v���C���X�e�[�g�̐ݒ�Ɏ��s\n");
		return;
	}

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
