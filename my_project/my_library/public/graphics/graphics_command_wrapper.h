#pragma once
#include <d3d12.h>

MY_LIB_NAMESPACE_BEGIN

class CGraphicsCommandWrapper
{
public:
	CGraphicsCommandWrapper();
	~CGraphicsCommandWrapper();

	/// @brief 初期化
	/// @return 成否
	bool Initialize();

	/// @brief 終了
	void Finalize();

	/// @brief リセット
	void Reset();

	/// @brief 実行
	void Execute();

	/// @brief リソースバリア
	/// @param _resource バリアを実行するリソース
	/// @param _prev 前状態
	/// @param _next 次状態
	void ResourceBarrier(ID3D12Resource* _resource, D3D12_RESOURCE_STATES _prev, D3D12_RESOURCE_STATES _next);

	/// @brief コマンドキューを取得
	/// @return コマンドキュー
	ID3D12CommandQueue* GetCommandQueue();

	/// @brief コマンドアロケータを取得
	/// @return コマンドアロケータ
	ID3D12CommandAllocator* GetCommandAllocator();

	/// @brief コマンドリストを取得
	/// @return コマンドリスト
	ID3D12GraphicsCommandList* GetCommandList();

private:
	/// @brief 開放
	void Release();

	/// @brief コマンドキューを作成
	/// @return 成否
	bool CreateD3d12CommandQueue();

	/// @brief コマンドアロケータを作成
	/// @return 成否
	bool CreateD3d12CommandAllocator();

	/// @brief コマンドリストを作成
	/// @return 成否
	bool CreateD3d12CommandList();

private:
	ID3D12CommandQueue* m_commandQueue = nullptr;				// コマンドキュー
	ID3D12CommandAllocator* m_commandAllocator = nullptr;		// コマンドアロケータ
	ID3D12GraphicsCommandList* m_commandList = nullptr;			// コマンドリスト
};

MY_LIB_NAMESPACE_END
