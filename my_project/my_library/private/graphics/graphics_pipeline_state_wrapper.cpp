#include "graphics/graphics_pipeline_state_wrapper.h"
#include "shader/shader.h"

MY_LIB_NAMESPACE_BEGIN

CGraphicsPipelineStateWrapper::CGraphicsPipelineStateWrapper()
{

}

CGraphicsPipelineStateWrapper::~CGraphicsPipelineStateWrapper()
{

}

/// @brief 初期化
/// @param _initData 初期化データ
/// @return 成否
bool CGraphicsPipelineStateWrapper::Initialize(const InitData& _initData)
{
	if (!CreateRootSignature(_initData))
	{
		printf("CGraphicsPipelineStateWrapper::Initialize ルートシグネチャの作成に失敗\n");
		return false;
	}

	if (!CreatePipelineState(_initData))
	{
		printf("CGraphicsPipelineStateWrapper::Initialize パイプラインステートの作成に失敗\n");
		return false;
	}

	return true;
}

/// @brief 終了
void CGraphicsPipelineStateWrapper::Finalize()
{
	Release();
}

/// @brief ルートシグネチャとパイプラインステートを設定する
/// @param _commandList 設定するコマンドリスト
/// @return 成否
bool CGraphicsPipelineStateWrapper::SetRootSignatureAndPipelineState(ID3D12GraphicsCommandList* _commandList)
{
	if (_commandList == nullptr)
	{
		printf("CGraphicsPipelineStateWrapper::SetPipelineState _commandListがnullです\n");
		return false;
	}

	_commandList->SetGraphicsRootSignature(m_rootSignature);
	_commandList->SetPipelineState(m_pipelineState);

	return true;
}

/// @brief 開放
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

/// @brief ルートシグネチャ作成
/// @param _initData 初期化データ
/// @return 成否
bool CGraphicsPipelineStateWrapper::CreateRootSignature(const InitData& _initData)
{
	if (_initData.d3dDevice == nullptr)
	{
		printf("CGraphicsPipelineStateWrapper::CreateRootSignature d3dデバイスがnullです\n");
		return false;
	}

	// todo 一旦ルートパラメータ等は空で作成（必要になった際に作成する）
	//ルートシグネチャを作成
	D3D12_ROOT_SIGNATURE_DESC desc;
	desc.NumParameters = 0;								//ルートパラメータ数
	desc.pParameters = nullptr;							//ルートパラメータのポインタ
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
	result = _initData.d3dDevice->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
	if (result != S_OK || m_rootSignature == nullptr)
	{
		printf("CModel::CreateRootSignature ルートシグネチャの生成に失敗\n");
		return false;
	}

	return true;
}

/// @brief パイプラインステート作成
/// @param _initData 初期化データ
/// @return 成否
bool CGraphicsPipelineStateWrapper::CreatePipelineState(const InitData& _initData)
{
	if (_initData.d3dDevice == nullptr)
	{
		printf("CGraphicsPipelineStateWrapper::CreatePipelineState d3dデバイスがnullです\n");
		return false;
	}

	if (_initData.vsShader == nullptr)
	{
		printf("CGraphicsPipelineStateWrapper::CreatePipelineState vsShaderがnullです\n");
		return false;
	}

	if (_initData.psShader == nullptr)
	{
		printf("CGraphicsPipelineStateWrapper::CreatePipelineState psShaderがnullです\n");
		return false;
	}

	ID3DBlob* vsBlob = _initData.vsShader->GetShader();
	ID3DBlob* psBlob = _initData.psShader->GetShader();
	if (vsBlob == nullptr || psBlob == nullptr)
	{
		printf("CGraphicsPipelineStateWrapper::CreatePipelineState シェーダーの取得に失敗\n");
		return false;
	}

	// todo ひとまずCModelでしていた設定をそのまま持ってきているが、必要になったらtype的なものを渡して設定を切り替えられるようにする
	
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
		printf("CGraphicsPipelineStateWrapper::CreatePipelineState パイプラインステートの作成に失敗。\n");
		return false;
	}

	return true;
}

MY_LIB_NAMESPACE_END
