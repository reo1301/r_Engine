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
	// ���_�o�b�t�@
	m_vertexBuffer.Finalize();

	// �p�C�v���C���X�e�[�g
	m_pipelineState.Finalize();

	// �V�F�[�_�[
	m_vertexShader.Release();
	m_pixelShader.Release();
}

/// @brief ������
/// @param _initData �������f�[�^
/// @return ����
bool CModel::Initialize(const ModelInitData& _initData)
{
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
	if (!m_pipelineState.Initialize(pipelineStateInitData))
	{
		printf("CModel::Initialize �p�C�v���C���X�e�[�g�̏������Ɏ��s\n");
		return false;
	}

	// ���_�o�b�t�@�쐬
	CVertexBuffer::InitData vertexBufferInitData;
	vertexBufferInitData.d3dDevice = d3dDevice;
	vertexBufferInitData.vertexData = _initData.vertexData;
	if (!m_vertexBuffer.Initialize(vertexBufferInitData))
	{
		printf("CModel::Initialize ���_�o�b�t�@�̏������Ɏ��s\n");
		return false;
	}

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

	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewList[] = { m_vertexBuffer.GetVertexBufferView() };
	// �h���[�R�[�����R�}���h���X�g�ɐς�
	d3dGraphicsCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	d3dGraphicsCommandList->IASetVertexBuffers(0, _countof(vertexBufferViewList), vertexBufferViewList);
	d3dGraphicsCommandList->DrawInstanced(m_vertexBuffer.GetVertexCount(), 1, 0, 0);
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
