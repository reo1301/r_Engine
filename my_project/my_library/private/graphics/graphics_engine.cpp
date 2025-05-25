#include "graphics/graphics_engine.h"

#include "window/gui_window.h"

MY_LIB_NAMESPACE_BEGIN

#define REFRESH_RATE		60			//リフレッシュレート

CGraphicsEngine* CGraphicsEngine::s_instance = nullptr;

/// @brief コンストラクタ
CGraphicsEngine::CGraphicsEngine()
{

}

/// @brief デストラクタ
CGraphicsEngine::~CGraphicsEngine()
{

}

/// @brief 初期化処理
void CGraphicsEngine::Initialize()
{
#ifdef _DEBUG
	InitDebugLayer();
#endif

	// DXGIファクトリの作成
	CreateDxgiFactory();

	// D3D12デバイスを作成
	CreateD3d12Device();

	// コマンドキューを作成
	CreateD3d12CommandQueue();

	// コマンドアロケータを作成
	CreateD3d12CommandAllocator();

	// コマンドリストを作成
	CreateD3d12CommandList();

	// スワップチェーンを作成
	CreateSwapChain();

	// RTV用のディスクリプタヒープを作成
	CreateD3d12RtvDescriptorHeap();

	// レンダーターゲットを作成
	CreateRenderTarget();

	// フェンスを作成
	CreateD3d12Fence();
}

/// @brief 終了処理
void CGraphicsEngine::Finalize()
{
	// 描画まち
	WaitDraw();

	// フェンスを削除
	if (m_d3d12Fence != nullptr)
	{
		m_d3d12Fence->Release();
		m_d3d12Fence = nullptr;
	}

	// RTVを削除
	for (auto& renderTarget : m_renderTarget)
	{
		if (renderTarget != nullptr)
		{
			renderTarget->Release();
			renderTarget = nullptr;
		}
	}

	// RTV用のディスクリプタヒープを削除
	if (m_d3d12RtvDescriptorHeap != nullptr)
	{
		m_d3d12RtvDescriptorHeap->Release();
		m_d3d12RtvDescriptorHeap = nullptr;
	}

	// スワップチェーンを削除
	if (m_dxgiSwapChain != nullptr)
	{
		m_dxgiSwapChain->Release();
		m_dxgiSwapChain = nullptr;
	}

	// コマンドリストを削除
	if (m_d3d12GraphicsCommandList != nullptr)
	{
		m_d3d12GraphicsCommandList->Release();
		m_d3d12GraphicsCommandList = nullptr;
	}

	// コマンドアロケータを削除
	if (m_d3d12CommandAllocator != nullptr)
	{
		m_d3d12CommandAllocator->Release();
		m_d3d12CommandAllocator = nullptr;
	}

	// コマンドキューを削除
	if (m_d3d12CommandQueue != nullptr)
	{
		m_d3d12CommandQueue->Release();
		m_d3d12CommandQueue = nullptr;
	}

	// D3D12デバイスを削除
	if (m_d3d12Device != nullptr)
	{
		m_d3d12Device->Release();
		m_d3d12Device = nullptr;
	}

	// DXGIファクトリを削除
	if (m_dxgiFactory != nullptr)
	{
		m_dxgiFactory->Release();
		m_dxgiFactory = nullptr;
	}
}

/// @brief 更新
void CGraphicsEngine::Update()
{
	HRESULT result;

	if (m_d3d12CommandAllocator == nullptr)
	{
		printf("CGraphicsEngine::Update m_d3d12CommandAllocatorがnullです。\n");
	}

	if (m_d3d12GraphicsCommandList == nullptr)
	{
		printf("CGraphicsEngine::Update m_d3d12GraphicsCommandListがnullです。\n");
	}
	
	m_frameIdx = m_dxgiSwapChain->GetCurrentBackBufferIndex();

	// todo いったんGraphicsEngineですべて描画までやるが、コマンド周りは機能分けする
	// todo Draw関数なども作成して実行順に気を付ける

	// コマンドアロケータをリセット
	result = m_d3d12CommandAllocator->Reset();
	assert(SUCCEEDED(result));

	// コマンドリストをリセット
	result = m_d3d12GraphicsCommandList->Reset(m_d3d12CommandAllocator, nullptr);
	assert(SUCCEEDED(result));

	// todo PreUpdateなどで、描画前に実行
	// レンダーターゲットとして使用できるようになるまで待つ
	{
		D3D12_RESOURCE_BARRIER barrier;
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;		//サブリソースの遷移
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		{
			D3D12_RESOURCE_TRANSITION_BARRIER transition;
			transition.pResource = m_renderTarget[m_frameIdx];				//レンダーターゲット
			transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;			//遷移前の状態(Present)
			transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;		//遷移後の状態(描画ターゲット)
			transition.pResource = m_renderTarget[m_frameIdx];
			barrier.Transition = transition;
		}
		m_d3d12GraphicsCommandList->ResourceBarrier(1, &barrier);
	}

	// レンダーターゲットをセット
	SetRenderTarget();

	// バックバッファをクリア
	ClearBackBuffer();

	// todo 描画コマンドを全て積んだ後に実行
	// バックバッファの描画が完了するまで待つ
	{
		D3D12_RESOURCE_BARRIER barrier;
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;		// サブリソースの遷移
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		{
			D3D12_RESOURCE_TRANSITION_BARRIER transition;
			transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;		// 遷移前の状態(描画ターゲット)
			transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;				// 遷移後の状態(Present)
			transition.pResource = m_renderTarget[m_frameIdx];
			barrier.Transition = transition;
		}
		m_d3d12GraphicsCommandList->ResourceBarrier(1, &barrier);
	}

	// コマンドリストをクローズする
	m_d3d12GraphicsCommandList->Close();

	// コマンドリストを実行
	ID3D12CommandList* commandLists[] = { m_d3d12GraphicsCommandList };
	m_d3d12CommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	// 画面のフリップ
	m_dxgiSwapChain->Present(1, 0);

	WaitDraw();
}

#ifdef _DEBUG

/// @brief デバッグレイヤーの初期化
void CGraphicsEngine::InitDebugLayer() const
{
	ID3D12Debug* debugController = nullptr;

	//デバッグコントローラーの生成
	HRESULT result = S_OK;
	result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));

	if (result != S_OK || debugController == nullptr)
	{
		printf("CGraphicsEngine::InitDebugLayer デバッグコントローラーの生成に失敗\n");
		return;
	}

	//デバッグコントローラーの有効化
	debugController->EnableDebugLayer();

	debugController->Release();
}

#endif

/// @brief DXGIオブジェクトの生成に使用するDXGIファクトリの作成
void CGraphicsEngine::CreateDxgiFactory()
{
	//DXGIファクトリの生成
	HRESULT result = S_OK;
	result = CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory));

	if (result != S_OK || m_dxgiFactory == nullptr)
	{
		printf("CGraphicsEngine::CreateDxgiFactory DXGIファクトリの生成に失敗\n");
		return;
	}
}

/// @brief D3D12デバイスを作成
void CGraphicsEngine::CreateD3d12Device()
{
	//D3D12デバイスの生成
	HRESULT result = S_OK;
	//WARPアダプタは指定しない
	result = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_d3d12Device));

	if (result != S_OK || m_d3d12Device == nullptr)
	{
		printf("CGraphicsEngine::CreateD3d12Device D3D12デバイスの生成に失敗\n");
		return;
	}
}

/// @brief コマンドキューを作成
void CGraphicsEngine::CreateD3d12CommandQueue()
{
	if (m_d3d12Device == nullptr)
	{
		printf("CGraphicsEngine::CreateD3d12CommandQueue d3dデバイスがnullです\n");
		return;
	}

	//コマンドキューを作成
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;					//ダイレクトコマンドキュー
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;		//通常の優先度
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;					//GPUタイムアウトが有効
	desc.NodeMask = 0;											//GPUは1つのみ

	HRESULT result;
	result = m_d3d12Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_d3d12CommandQueue));
	if (result != S_OK || m_d3d12CommandQueue == nullptr)
	{
		printf("CGraphicsEngine::CreateD3d12CommandQueue D3D12コマンドキューの生成に失敗\n");
		return;
	}
}

/// @brief スワップチェーンを作成
void CGraphicsEngine::CreateSwapChain()
{
	if (m_dxgiFactory == nullptr)
	{
		printf("CGraphicsEngine::CreateSwapChain dxgiファクトリがnullです\n");
		return;
	}
	if (m_d3d12CommandQueue == nullptr)
	{
		printf("CGraphicsEngine::CreateSwapChain コマンドキューがnullです\n");
		return;
	}

	const CGUIWindow& guiWindow = CGUIWindow::GetInstance();
	DXGI_MODE_DESC modeDesc = {};
	{
		modeDesc.Width = guiWindow.GetWindowWidth();		//解像度の横幅
		modeDesc.Height = guiWindow.GetWindowHeight();		//解像度の縦幅
	}
	{
		//リフレッシュレート
		DXGI_RATIONAL refreshRate = {};
		refreshRate.Numerator = REFRESH_RATE;
		refreshRate.Denominator = 1;
		modeDesc.RefreshRate = refreshRate;
	}
	modeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;							//R8G8B8A8の符号なし正規化整数型
	modeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;		//スキャンライン指定なし
	modeDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;							//スケーリング指定なし

	DXGI_SAMPLE_DESC sampleDesc = {};
	sampleDesc.Count = 1;		//ピクセルごとのマルチサンプル数
	sampleDesc.Quality = 0;		//マルチサンプルはしない

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc = modeDesc;
	swapChainDesc.SampleDesc = sampleDesc;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;					//レンダーターゲットとして使用
	swapChainDesc.BufferCount = RTV_NUM;											//バッファー数
	swapChainDesc.OutputWindow = guiWindow.GetInstance().GetGuiWindowHandle();		//出力ウィンドウのハンドル
	swapChainDesc.Windowed = true;													//全画面表示
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;						//反転後にバックバッファの内容を破棄

	//スワップチェーンを作成
	HRESULT result;
	IDXGISwapChain* swapChain = nullptr;
    result = m_dxgiFactory->CreateSwapChain(m_d3d12CommandQueue, &swapChainDesc, &swapChain);
	if (result != S_OK || swapChain == nullptr)
	{
		printf("CGraphicsEngine::CreateSwapChain スワップチェーンの作成に失敗\n");
		return;
	}

	result = swapChain->QueryInterface(IID_PPV_ARGS(&m_dxgiSwapChain));
	if (result != S_OK || m_dxgiSwapChain == nullptr)
	{
		printf("CGraphicsEngine::CreateSwapChain スワップチェーンの型変換に失敗\n");
		return;
	}
	swapChain->Release();

	m_frameIdx = m_dxgiSwapChain->GetCurrentBackBufferIndex();
}

/// @brief ルートシグネチャを作成
void CGraphicsEngine::CreateRootSignature()
{
	if (m_d3d12Device == nullptr)
	{
		printf("CGraphicsEngine::CreateRootSignature d3dデバイスがnullです\n");
		return;
	}

	D3D12_DESCRIPTOR_RANGE descriptorRanges[1] = {};
	descriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;		//定数バッファとして使用
	descriptorRanges[0].NumDescriptors = 1;									//ディスクリプタは1つ
	descriptorRanges[0].BaseShaderRegister = 0;								//シェーダーレジスタの開始インデックスは0
	descriptorRanges[0].RegisterSpace = 0;									//レジスタ領域は使用しないので0
	descriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable = {};
	descriptorTable.NumDescriptorRanges = 1;		//ディスクリプタレンジの数は1
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
		printf("CGraphicsEngine::CreateRootSignature ルートシグネチャのシリアライズに失敗\n");
		return;
	}

	//ルートシグネチャの作成
	result = m_d3d12Device->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&m_d3d12RootSignature));
	if (result != S_OK || m_d3d12RootSignature == nullptr)
	{
		printf("CGraphicsEngine::CreateRootSignature ルートシグネチャの生成に失敗\n");
		return;
	}
}

/// @brief コマンドアロケータを作成
void CGraphicsEngine::CreateD3d12CommandAllocator()
{
	if (m_d3d12Device == nullptr)
	{
		printf("CGraphicsEngine::CreateD3d12CommandAllocator d3dデバイスがnullです\n");
		return;
	}

	HRESULT result;
	result = m_d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_d3d12CommandAllocator));
	if (result != S_OK || m_d3d12CommandAllocator == nullptr)
	{
		printf("CGraphicsEngine::CreateD3d12CommandAllocator コマンドアロケータの生成に失敗\n");
		return;
	}
}

/// @brief コマンドリストを作成
void CGraphicsEngine::CreateD3d12CommandList()
{
	if (m_d3d12CommandAllocator == nullptr)
	{
		printf("CGraphicsEngine::CreateD3d12CommandList コマンドアロケータがnullです\n");
		return;
	}
	if (m_d3d12Device == nullptr)
	{
		printf("CGraphicsEngine::CreateD3d12CommandList d3dデバイスがnullです\n");
		return;
	}

	HRESULT result;
	result = m_d3d12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_d3d12CommandAllocator, nullptr, IID_PPV_ARGS(&m_d3d12GraphicsCommandList));
	if (result != S_OK || m_d3d12GraphicsCommandList == nullptr)
	{
		printf("CGraphicsEngine::CreateD3d12CommandList コマンドリストの生成に失敗\n");
		return;
	}

	//コマンドリストをクローズしておく
	m_d3d12GraphicsCommandList->Close();
}

/// @brief RTV用ディスクリプタヒープを作成
void CGraphicsEngine::CreateD3d12RtvDescriptorHeap()
{
	if (m_d3d12Device == nullptr)
	{
		printf("CGraphicsEngine::CreateD3d12DescriptorHeap d3dデバイスがnullです\n");
		return;
	}

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;		//レンダーターゲットビューとして使用
	desc.NumDescriptors = RTV_NUM;					//フレームバッファとバックバッファで2
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	desc.NodeMask = 0;

	HRESULT result;
	result = m_d3d12Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_d3d12RtvDescriptorHeap));
	if (result != S_OK || m_d3d12RtvDescriptorHeap == nullptr)
	{
		printf("CGraphicsEngine::CreateD3d12DescriptorHeap ディスクリプタヒープの生成に失敗\n");
		return;
	}
}

/// @brief レンダーターゲットの作成
void CGraphicsEngine::CreateRenderTarget()
{
	//スワップチェーンのバッファをディスクリプタヒープに登録
	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_d3d12RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	const unsigned int rtvDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	for (unsigned int idx = 0; idx < RTV_NUM; idx++)
	{
		HRESULT result;
		result = m_dxgiSwapChain->GetBuffer(idx, IID_PPV_ARGS(&m_renderTarget[idx]));
		if (result != S_OK)
		{
			printf("CGraphicsEngine::CreateRenderTarget スワップチェーンのバッファアクセスに失敗 idx = %u\n", idx);
			continue;
		}
		//レンダーターゲットビューを作成
		m_d3d12Device->CreateRenderTargetView(m_renderTarget[idx], nullptr, handle);

		//ハンドルの指すポインタをディスクリプタ分加算
		handle.ptr += rtvDescriptorSize;
	}
}

/// @brief フェンスの作成
void CGraphicsEngine::CreateD3d12Fence()
{
	HRESULT result;
	result = m_d3d12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_d3d12Fence));
	if (result != S_OK || m_d3d12Fence == nullptr)
	{
		printf("CGraphicsEngine::CreateD3d12Fence フェンスの作成に失敗\n");
		return;
	}

	m_fenceValue = 1;
	
	// 同期を行う時のイベントハンドルを作成
	m_fenceEvent = CreateEvent(nullptr, false, false, nullptr);
	if (m_fenceEvent == nullptr)
	{
		printf("CGraphicsEngine::CreateD3d12Fence フェンスイベントハンドルの作成に失敗\n");
		return;
	}
}

/// @brief レンダーターゲットのセット
void CGraphicsEngine::SetRenderTarget()
{
	const unsigned int rtvDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE currentFrameBufferRTVHandle = m_d3d12RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	currentFrameBufferRTVHandle.ptr += rtvDescriptorSize * m_frameIdx;
	m_d3d12GraphicsCommandList->OMSetRenderTargets(1, &currentFrameBufferRTVHandle, false, nullptr);
}

/// @brief バックバッファのクリア
void CGraphicsEngine::ClearBackBuffer()
{
	if (m_dxgiSwapChain == nullptr)
	{
		printf("CGraphicsEngine::ClearBackBuffer m_dxgiSwapChainがnullです。\n");
		return;
	}

	const unsigned int rtvDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE currentFrameBufferRTVHandle = m_d3d12RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	currentFrameBufferRTVHandle.ptr += rtvDescriptorSize * m_frameIdx;

	// クリア
	m_d3d12GraphicsCommandList->ClearRenderTargetView(currentFrameBufferRTVHandle, RT_CLEAR_COLOR, 0, nullptr);
}

/// @brief 描画待機
void CGraphicsEngine::WaitDraw()
{
	if (m_d3d12CommandQueue == nullptr)
	{
		printf("CGraphicsEngine::WaitDraw m_d3d12CommandQueueがnullです\n");
	}

	if (m_d3d12Fence == nullptr)
	{
		printf("CGraphicsEngine::WaitDraw m_d3d12Fenceがnullです\n");
	}

	HRESULT result;
	const unsigned long long nowFenceValue = m_fenceValue;
	result = m_d3d12CommandQueue->Signal(m_d3d12Fence, nowFenceValue);
	m_fenceValue++;

	if (result != S_OK)
	{
		printf("CGraphicsEngine::WaitDraw 描画待機の同期に失敗\n");
		return;
	}

	const unsigned long long compValue = m_d3d12Fence->GetCompletedValue();
	if (compValue < nowFenceValue)
	{
		result = m_d3d12Fence->SetEventOnCompletion(nowFenceValue, m_fenceEvent);
		if (result != S_OK)
		{
			printf("CGraphicsEngine::WaitDraw フェンスが特定の値に達した際に発生するイベントの指定に失敗\n");
			return;
		}

		WaitForSingleObject(m_fenceEvent, INFINITE);
	}
}

/// @brief シングルトンのインスタンス生成
void CGraphicsEngine::CreateInstance()
{
	if (!s_instance)
	{
		s_instance = new CGraphicsEngine();
	}
}

/// @brief シングルトンのインスタンス取得
/// @return GraphicsEngineのインスタンスの参照
CGraphicsEngine& CGraphicsEngine::GetInstance()
{
	if (!s_instance)
	{
		throw;
	}
	return *s_instance;
}

/// @brief シングルトンのインスタンス削除
void CGraphicsEngine::Delete()
{
	if (s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

MY_LIB_NAMESPACE_END