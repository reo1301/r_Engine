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

	ID3DBlob* vsBlob = m_vertexShader.GetShader();
	ID3DBlob* psBlob = m_pixelShader.GetShader();
	if (vsBlob == nullptr || psBlob == nullptr)
	{
		printf("CModel::Initialize シェーダーの取得に失敗\n");
		return false;
	}

	// ルートシグネチャー作成
	if (!CreateRootSignature())
	{
		printf("CModel::Initialize ルートシグネチャの作成に失敗\n");
		return false;
	}

	// パイプラインステート作成
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

	// レンダーターゲットのブレンド状態
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

	// ブレンド状態
	D3D12_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0] = rtBlendDesc;

	// ラスタライザー
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

	// デプスステンシル
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	depthStencilDesc.StencilEnable = false;

	// パイプラインステート
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
		printf("CModel::Initialize D3DDeviceが無効です。\n");
		return false;
	}

	HRESULT result = d3dDevice->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&m_pipelineState));
	if (FAILED(result))
	{
		printf("CModel::Initialize パイプラインステートの作成に失敗。\n");
		return false;
	}

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

	result = d3dDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_vertexBuffer));
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
	d3dGraphicsCommandList->SetGraphicsRootSignature(m_rootSignature);
	d3dGraphicsCommandList->SetPipelineState(m_pipelineState);

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

/// @return 成否
bool CModel::CreateRootSignature()
{
	ID3D12Device* d3dDevice = CGraphicsEngine::GetInstance().GetD3dDevice();
	if (d3dDevice == nullptr)
	{
		printf("CModel::CreateRootSignature d3dデバイスの取得に失敗\n");
		return false;
	}

	D3D12_DESCRIPTOR_RANGE descriptorRanges[1] = {};
	descriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;		//定数バッファとして使用
	descriptorRanges[0].NumDescriptors = 1;									//ディスクリプタは1つ
	descriptorRanges[0].BaseShaderRegister = 0;								//シェーダーレジスタの開始インデックスは0
	descriptorRanges[0].RegisterSpace = 0;									//レジスタ領域は使用しないので0
	descriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable = {};
	descriptorTable.NumDescriptorRanges = 1;
	descriptorTable.pDescriptorRanges = descriptorRanges;

	D3D12_ROOT_PARAMETER rootParameters[1] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//定数バッファとして使用
	rootParameters[0].DescriptorTable = descriptorTable;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		//すべてのシェーダーからアクセス可能

	//ルートシグネチャを作成
	D3D12_ROOT_SIGNATURE_DESC desc;
	desc.NumParameters = _countof(rootParameters);		//ルートパラメータ数
	desc.pParameters = rootParameters;					//ルートパラメータのポインタ
	desc.NumStaticSamplers = 0;							//サンプラー数
	desc.pStaticSamplers = nullptr;						//サンプラーのポインタ
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;		//オプション

	HRESULT result;
	//ルートシグネチャのシリアライズ
	ID3DBlob* pSignature = nullptr;
	result = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &pSignature, nullptr);
	if (result != S_OK || pSignature == nullptr)
	{
		printf("CModel::CreateRootSignature ルートシグネチャのシリアライズに失敗\n");
		return false;
	}

	//ルートシグネチャの作成
	result = d3dDevice->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
	if (result != S_OK || m_rootSignature == nullptr)
	{
		printf("CModel::CreateRootSignature ルートシグネチャの生成に失敗\n");
		return false;
	}

	return true;
}

/// @brief ディスクリプタヒープを作成
/// @return 成否
bool CModel::CreateDescriptorHeap()
{
	return true;
}
