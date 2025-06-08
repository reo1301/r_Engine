#include "graphics/model/model.h"

#include "graphics/model/model_utility.h"
#include "graphics/graphics_engine.h"

CModel::CModel()
{
}

CModel::~CModel()
{
}

/// @brief 開放
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

/// @brief 初期化
/// @param _initData 初期化データ
/// @return 成否
bool CModel::Initialize(const ModelInitData& _initData)
{
	const VertexData& vertexData = _initData.vertexData;
	if ((vertexData.vertexDataPtr == nullptr) || (vertexData.vertexDataSize == 0) || (vertexData.vertexDataStride == 0))
	{
		printf("CModel::Initialize 頂点データが無効\n");
		return false;
	}

	if ((_initData.inputLayoutType == MODEL_INPUT_LAYOUT_TYPE_NONE) || (_initData.inputLayoutType >= MODEL_INPUT_LAYOUT_TYPE_NUM))
	{
		printf("CModel::Initialize inputLayoutTypeが無効です\n");
		return false;
	}

	// todo シェーダーは毎回作るのではなく作成済みのものを渡すか、どこかで収集しておいて検索する形にした方がよさそう
	// シェーダーのロード
	if (!LoadShader(_initData.vsInitData, _initData.psInitData))
	{
		printf("CModel::Initialize シェーダーのロードに失敗\n");
		return false;
	}

	// 入力レイアウトを取得
	const MODEL_INPUT_LAYOUT_ELEMENT* inputLayout = nullptr;
	unsigned int elementNum = 0;
	if (!CModelUtility::GetModelInputLayout(inputLayout, elementNum, _initData.inputLayoutType) || (inputLayout == nullptr))
	{
		printf("CModel::Initialize モデルの入力レイアウトの取得に失敗。inputLayoutType = %u\n", _initData.inputLayoutType);
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
		printf("CModel::Initialize D3DDeviceが無効です。\n");
		return false;
	}

	// パイプラインステート作成
	CGraphicsPipelineStateWrapper::InitData pipelineStateInitData;
	pipelineStateInitData.d3dDevice = d3dDevice;
	pipelineStateInitData.vsShader = &m_vertexShader;
	pipelineStateInitData.psShader = &m_pixelShader;
	pipelineStateInitData.inputElementList = inputDesc;
	pipelineStateInitData.inputElementNum = elementNum;
	m_pipelineState.Initialize(pipelineStateInitData);

	// ヒーププロパティ
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.CreationNodeMask = 1;
	heapProp.VisibleNodeMask = 1;

	// リソース状態
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
		printf("CModel::Initialize 頂点バッファの作成に失敗。\n");
		return false;
	}

	// 頂点バッファにコピー
	void* vertexPtr = nullptr;
	result = m_vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vertexPtr));
	if (FAILED(result))
	{
		printf("CModel::Initialize 頂点バッファのマップに失敗\n");
		return false;
	}
	memcpy_s(vertexPtr, vertexData.vertexDataSize, vertexData.vertexDataPtr, vertexData.vertexDataSize);
	m_vertexBuffer->Unmap(0, nullptr);

	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = static_cast<UINT>(vertexData.vertexDataSize);
	m_vertexBufferView.StrideInBytes = static_cast<UINT>(vertexData.vertexDataStride);

	return true;
}

/// @brief 更新
void CModel::Update()
{

}

/// @brief 描画
void CModel::Draw()
{
	CGraphicsEngine& graphicsEngine = CGraphicsEngine::GetInstance();
	ID3D12GraphicsCommandList* d3dGraphicsCommandList = graphicsEngine.GetD3dGraphicsCommandList();
	if (d3dGraphicsCommandList == nullptr)
	{
		printf("CModel::Draw d3dGraphicsCommandListの取得に失敗\n");
		return;
	}

	if (!m_pipelineState.SetRootSignatureAndPipelineState(d3dGraphicsCommandList))
	{
		printf("CModel::Draw ルートシグネチャとパイプラインステートの設定に失敗\n");
		return;
	}

	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewList[] = { m_vertexBufferView };
	// ドローコールをコマンドリストに積む
	d3dGraphicsCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	d3dGraphicsCommandList->IASetVertexBuffers(0, _countof(vertexBufferViewList), vertexBufferViewList);
	d3dGraphicsCommandList->DrawInstanced(3, 1, 0, 0);
}

/// @brief 後更新
void CModel::PostUpdate()
{

}

/// @brief シェーダーのロード
/// @param _vsInitData 頂点シェーダー初期化データ
/// @param _psInitData ピクセルシェーダー初期化データ
/// @return 成否
bool CModel::LoadShader(const CShader::ShaderInitData& _vsInitData, const CShader::ShaderInitData& _psInitData)
{
	// 頂点シェーダー
	if (!m_vertexShader.Load(_vsInitData))
	{
		printf("CModel::Initialize 頂点シェーダーのロードに失敗\n");
		return false;
	}

	// ピクセルシェーダー
	if (!m_pixelShader.Load(_psInitData))
	{
		printf("CModel::Initialize ピクセルシェーダーのロードに失敗\n");
		return false;
	}

	return true;
}
