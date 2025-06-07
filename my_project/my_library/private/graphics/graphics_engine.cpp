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
/// @param _windowWidth ウィンドウの横幅
/// @param _windowHeight ウィンドウの縦幅
void CGraphicsEngine::Initialize(unsigned int _windowWidth, unsigned int _windowHeight)
{
	m_windowWidth = _windowWidth;
	m_windowHeight = _windowHeight;

#ifdef _DEBUG
	InitDebugLayer();
#endif

	// DXGIファクトリの作成
	CreateDxgiFactory();

	// D3D12デバイスを作成
	CreateD3d12Device();

	m_commandWrapper.Initialize(m_d3d12Device);

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

	// コマンド
	m_commandWrapper.Finalize();

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

/// @brief 前更新
void CGraphicsEngine::PreUpdate()
{
	// コマンドのリセット
	m_commandWrapper.Reset();

	m_frameIdx = m_dxgiSwapChain->GetCurrentBackBufferIndex();

	// レンダーターゲットとして使用できるようになるまで待つ(Present -> 描画ターゲット)
	m_commandWrapper.ResourceBarrier(m_renderTarget[m_frameIdx], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	ID3D12GraphicsCommandList* graphicsCommandList = GetD3dGraphicsCommandList();
	if (graphicsCommandList != nullptr)
	{
		// ビューポートの設定
		D3D12_VIEWPORT viewport;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = static_cast<float>(m_windowWidth);
		viewport.Height = static_cast<float>(m_windowHeight);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		graphicsCommandList->RSSetViewports(1, &viewport);

		// シザー矩形の設定
		D3D12_RECT rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = m_windowWidth;
		rect.bottom = m_windowHeight;
		graphicsCommandList->RSSetScissorRects(1, &rect);
	}
}

/// @brief 更新
void CGraphicsEngine::Update()
{
	if (m_d3d12Device == nullptr)
	{
		printf("CGraphicsEngine::Update m_d3d12Deviceがnullです。\n");
		return;
	}

	if (m_d3d12RtvDescriptorHeap == nullptr)
	{
		printf("CGraphicsEngine::Update m_d3d12RtvDescriptorHeapがnullです。\n");
		return;
	}

	// レンダーターゲットをセット
	SetRenderTarget();

	// バックバッファをクリア
	ClearBackBuffer();
}

/// @brief 描画
void CGraphicsEngine::Draw()
{

}

/// @brief 後更新
void CGraphicsEngine::PostUpdate()
{
	// バックバッファの描画が完了するまで待つ(描画ターゲット -> Present)
	m_commandWrapper.ResourceBarrier(m_renderTarget[m_frameIdx], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	// コマンド実行
	m_commandWrapper.Execute();

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

/// @brief スワップチェーンを作成
void CGraphicsEngine::CreateSwapChain()
{
	if (m_dxgiFactory == nullptr)
	{
		printf("CGraphicsEngine::CreateSwapChain dxgiファクトリがnullです\n");
		return;
	}
	ID3D12CommandQueue* graphicsCommandQueue = m_commandWrapper.GetCommandQueue();
	if (graphicsCommandQueue == nullptr)
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
    result = m_dxgiFactory->CreateSwapChain(graphicsCommandQueue, &swapChainDesc, &swapChain);
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
	ID3D12GraphicsCommandList* graphicsCommandList = GetD3dGraphicsCommandList();
	if (graphicsCommandList == nullptr)
	{
		printf("CGraphicsEngine::SetRenderTarget graphicsCommandListがnullです\n");
		return;
	}

	const unsigned int rtvDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE currentFrameBufferRTVHandle = m_d3d12RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	currentFrameBufferRTVHandle.ptr += rtvDescriptorSize * m_frameIdx;
	graphicsCommandList->OMSetRenderTargets(1, &currentFrameBufferRTVHandle, false, nullptr);
}

/// @brief バックバッファのクリア
void CGraphicsEngine::ClearBackBuffer()
{
	ID3D12GraphicsCommandList* graphicsCommandList = GetD3dGraphicsCommandList();
	if (graphicsCommandList == nullptr)
	{
		printf("CGraphicsEngine::SetRenderTarget graphicsCommandListがnullです\n");
		return;
	}

	const unsigned int rtvDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE currentFrameBufferRTVHandle = m_d3d12RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	currentFrameBufferRTVHandle.ptr += rtvDescriptorSize * m_frameIdx;

	// クリア
	graphicsCommandList->ClearRenderTargetView(currentFrameBufferRTVHandle, RT_CLEAR_COLOR, 0, nullptr);
}

/// @brief 描画待機
void CGraphicsEngine::WaitDraw()
{
	ID3D12CommandQueue* graphicsCommandQueue = m_commandWrapper.GetCommandQueue();
	if (graphicsCommandQueue == nullptr)
	{
		printf("CGraphicsEngine::WaitDraw graphicsCommandQueueがnullです\n");
	}

	if (m_d3d12Fence == nullptr)
	{
		printf("CGraphicsEngine::WaitDraw m_d3d12Fenceがnullです\n");
	}

	HRESULT result;
	const unsigned long long nowFenceValue = m_fenceValue;
	result = graphicsCommandQueue->Signal(m_d3d12Fence, nowFenceValue);
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

/// @brief D3Dデバイスを取得
/// @return D3Dデバイス
ID3D12Device* CGraphicsEngine::GetD3dDevice()
{
	return m_d3d12Device;
}

/// @brief D3Dグラフィックスコマンドリストを取得
/// @return D3Dグラフィックスコマンドリスト
ID3D12GraphicsCommandList* CGraphicsEngine::GetD3dGraphicsCommandList()
{
	return m_commandWrapper.GetCommandList();
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