#include "graphics/graphics_command_wrapper.h"

CGraphicsCommandWrapper::CGraphicsCommandWrapper()
{
}

CGraphicsCommandWrapper::~CGraphicsCommandWrapper()
{
	Finalize();
}

/// @brief 初期化
/// @param _d3dDevice d3dデバイス
/// @return 成否
bool CGraphicsCommandWrapper::Initialize(ID3D12Device* _d3dDevice)
{
	// コマンドキューを作成
	if (!CreateD3d12CommandQueue(_d3dDevice))
	{
		printf("CGraphicsCommandWrapper::Initialize コマンドキューの作成に失敗\n");
		return false;
	}

	// コマンドアロケータを作成
	if (!CreateD3d12CommandAllocator(_d3dDevice))
	{
		printf("CGraphicsCommandWrapper::Initialize コマンドアロケータの作成に失敗\n");
		return false;
	}

	// コマンドリストを作成
	if (!CreateD3d12CommandList(_d3dDevice))
	{
		printf("CGraphicsCommandWrapper::Initialize コマンドリストの作成に失敗\n");
		return false;
	}

	return true;
}

/// @brief 終了
void CGraphicsCommandWrapper::Finalize()
{
	Release();
}

/// @brief リセット
void CGraphicsCommandWrapper::Reset()
{
	if (m_commandAllocator == nullptr)
	{
		printf("CGraphicsCommandWrapper::Reset m_commandAllocatorがnullです。\n");
		return;
	}

	if (m_commandList == nullptr)
	{
		printf("CGraphicsCommandWrapper::Reset m_commandListがnullです。\n");
		return;
	}

	// コマンドアロケータをリセット
	HRESULT result = m_commandAllocator->Reset();
	assert(SUCCEEDED(result));

	// コマンドリストをリセット
	result = m_commandList->Reset(m_commandAllocator, nullptr);
	assert(SUCCEEDED(result));
}

/// @brief 実行
void CGraphicsCommandWrapper::Execute()
{
	if (m_commandList == nullptr)
	{
		printf("CGraphicsCommandWrapper::Execute m_commandListがnullです。\n");
		return;
	}

	if (m_commandQueue == nullptr)
	{
		printf("CGraphicsCommandWrapper::Execute m_commandQueueがnullです。\n");
		return;
	}

	// コマンドリストをクローズする
	m_commandList->Close();

	// コマンドリストを実行
	ID3D12CommandList* commandLists[] = { m_commandList };
	m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
}

/// @brief リソースバリア
/// @param _resource バリアを実行するリソース
/// @param _prev 前状態
/// @param _next 次状態
void CGraphicsCommandWrapper::ResourceBarrier(ID3D12Resource* _resource, D3D12_RESOURCE_STATES _prev, D3D12_RESOURCE_STATES _next)
{
	if (_resource == nullptr)
	{
		printf("CGraphicsCommandWrapper::ResourceBarrier _resourceがnullです\n");
		return;
	}

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;		//サブリソースの遷移
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	{
		D3D12_RESOURCE_TRANSITION_BARRIER transition;
		transition.pResource = _resource;
		transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		transition.StateBefore = _prev;		//遷移前の状態
		transition.StateAfter = _next;		//遷移後の状態
		barrier.Transition = transition;
	}
	m_commandList->ResourceBarrier(1, &barrier);
}

/// @brief コマンドキューを取得
/// @return コマンドキュー
ID3D12CommandQueue* CGraphicsCommandWrapper::GetCommandQueue()
{
	return m_commandQueue;
}

/// @brief コマンドアロケータを取得
/// @return コマンドアロケータ
ID3D12CommandAllocator* CGraphicsCommandWrapper::GetCommandAllocator()
{
	return m_commandAllocator;
}

/// @brief コマンドリストを取得
/// @return コマンドリスト
ID3D12GraphicsCommandList* CGraphicsCommandWrapper::GetCommandList()
{
	return m_commandList;
}

/// @brief 開放
void CGraphicsCommandWrapper::Release()
{
	// コマンドリストを削除
	if (m_commandList != nullptr)
	{
		m_commandList->Release();
		m_commandList = nullptr;
	}

	// コマンドアロケータを削除
	if (m_commandAllocator != nullptr)
	{
		m_commandAllocator->Release();
		m_commandAllocator = nullptr;
	}

	// コマンドキューを削除
	if (m_commandQueue != nullptr)
	{
		m_commandQueue->Release();
		m_commandQueue = nullptr;
	}
}


/// @brief コマンドキューを作成
/// @param _d3dDevice d3dデバイス
/// @return 成否
bool CGraphicsCommandWrapper::CreateD3d12CommandQueue(ID3D12Device* _d3dDevice)
{
	if (m_commandQueue != nullptr)
	{
		printf("CGraphicsCommandWrapper::CreateD3d12CommandQueue コマンドキューは生成済みです\n");
		return true;
	}

	if (_d3dDevice == nullptr)
	{
		printf("CGraphicsCommandWrapper::CreateD3d12CommandQueue _d3dDeviceがnullです\n");
		return false;
	}

	//コマンドキューを作成
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;					//ダイレクトコマンドキュー
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;		//通常の優先度
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;					//GPUタイムアウトが有効
	desc.NodeMask = 0;											//GPUは1つのみ

	HRESULT result = _d3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue));
	if (result != S_OK || m_commandQueue == nullptr)
	{
		printf("CGraphicsCommandWrapper::CreateD3d12CommandQueue D3D12コマンドキューの生成に失敗\n");
		return false;
	}

	return true;
}

/// @brief コマンドアロケータを作成
/// @param _d3dDevice d3dデバイス
/// @return 成否
bool CGraphicsCommandWrapper::CreateD3d12CommandAllocator(ID3D12Device* _d3dDevice)
{
	if (m_commandAllocator != nullptr)
	{
		printf("CGraphicsCommandWrapper::CreateD3d12CommandAllocator コマンドアロケーターは生成済みです\n");
		return true;
	}

	if (_d3dDevice == nullptr)
	{
		printf("CGraphicsCommandWrapper::CreateD3d12CommandAllocator _d3dDeviceがnullです\n");
		return false;
	}

	HRESULT result = _d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));
	if (result != S_OK || m_commandAllocator == nullptr)
	{
		printf("CGraphicsCommandWrapper::CreateD3d12CommandAllocator コマンドアロケータの生成に失敗\n");
		return false;
	}

	return true;
}


/// @brief コマンドリストを作成
/// @param _d3dDevice d3dデバイス
/// @return 成否
bool CGraphicsCommandWrapper::CreateD3d12CommandList(ID3D12Device* _d3dDevice)
{
	if (m_commandList != nullptr)
	{
		printf("CGraphicsCommandWrapper::CreateD3d12CommandList コマンドリストは生成済みです\n");
		return true;
	}

	if (m_commandAllocator == nullptr)
	{
		printf("CGraphicsCommandWrapper::CreateD3d12CommandList m_commandAllocatorがnullです\n");
		return false;
	}

	if (_d3dDevice == nullptr)
	{
		printf("CGraphicsCommandWrapper::CreateD3d12CommandList d3dデバイスがnullです\n");
		return false;
	}

	HRESULT result = _d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator, nullptr, IID_PPV_ARGS(&m_commandList));
	if (result != S_OK || m_commandList == nullptr)
	{
		printf("CGraphicsCommandWrapper::CreateD3d12CommandList コマンドリストの生成に失敗\n");
		return false;
	}

	//コマンドリストをクローズしておく
	m_commandList->Close();

	return true;
}
