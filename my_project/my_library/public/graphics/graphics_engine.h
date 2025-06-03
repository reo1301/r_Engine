#pragma once

//インクルード
//コピー禁止
#include "../noncopyable.h"

//D3D系
#include <d3d12.h>
#include <dxgi1_4.h>

MY_LIB_NAMESPACE_BEGIN

#define RTV_NUM	2		//レンダーターゲットの数(フレームバッファとバックバッファで2)

class CGraphicsEngine : private Noncopyable
{
private:
	/// @brief コンストラクタ
	CGraphicsEngine();

public:
	/// @brief デストラクタ
	~CGraphicsEngine();

public:
	/// @brief 初期化処理
	/// @param _windowWidth ウィンドウの横幅
	/// @param _windowHeight ウィンドウの縦幅
	void Initialize(unsigned int _windowWidth, unsigned int _windowHeight);

	/// @brief 終了処理
	void Finalize();

	/// @brief 前更新
	void PreUpdate();

	/// @brief 更新
	void Update();

	/// @brief 描画
	void Draw();

	/// @brief 後更新
	void PostUpdate();

private:
#ifdef _DEBUG

	/// @brief デバッグレイヤーの初期化
	void InitDebugLayer() const;

#endif

	/// @brief DXGIオブジェクトの生成に使用するDXGIファクトリ作成
	void CreateDxgiFactory();

	/// @brief D3D12デバイスを作成
	void CreateD3d12Device();

	/// @brief コマンドキューを作成
	void CreateD3d12CommandQueue();

	/// @brief スワップチェーンを作成
	void CreateSwapChain();

	/// @brief コマンドアロケータを作成
	void CreateD3d12CommandAllocator();

	/// @brief コマンドリストを作成
	void CreateD3d12CommandList();

	/// @brief RTV用ディスクリプタヒープを作成
	void CreateD3d12RtvDescriptorHeap();

	/// @brief レンダーターゲットの作成
	void CreateRenderTarget();

	/// @brief フェンスの作成
	void CreateD3d12Fence();

	/// @brief レンダーターゲットのセット
	void SetRenderTarget();

	/// @brief バックバッファのクリア
	void ClearBackBuffer();

	/// @brief 描画待機
	void WaitDraw();

public:
	/// @brief D3Dデバイスを取得
	/// @return D3Dデバイス
	ID3D12Device* GetD3dDevice();

	/// @brief D3Dグラフィックスコマンドリストを取得
	/// @return D3Dグラフィックスコマンドリスト
	ID3D12GraphicsCommandList* GetD3dGraphicsCommandList();

private:
	static CGraphicsEngine* s_instance;		//シングルトンのインスタンス

public:
	/// @brief シングルトンのインスタンス生成
	static void CreateInstance();

	/// @brief シングルトンのインスタンス取得
	/// @return GraphicsEngineのインスタンスの参照
	static CGraphicsEngine& GetInstance();

	/// @brief シングルトンのインスタンス削除
	static void Delete();

private:
	static constexpr float RT_CLEAR_COLOR[4] = { 0.7f, 0.7f, 0.7f, 1.0f };

private:
	IDXGIFactory4* m_dxgiFactory = nullptr;									// DXGIファクトリ
	ID3D12Device* m_d3d12Device = nullptr;									// D3D12デバイス
	ID3D12CommandQueue* m_d3d12CommandQueue = nullptr;						// コマンドキュー
	IDXGISwapChain3* m_dxgiSwapChain = nullptr;								// スワップチェーン
	ID3D12CommandAllocator* m_d3d12CommandAllocator = nullptr;				// コマンドアロケータ
	ID3D12GraphicsCommandList* m_d3d12GraphicsCommandList = nullptr;		// コマンドリスト
	ID3D12DescriptorHeap* m_d3d12RtvDescriptorHeap = nullptr;				// RTV用ディスクリプタヒープ
	ID3D12Resource* m_renderTarget[RTV_NUM] = {};							// レンダーターゲット
	ID3D12Fence* m_d3d12Fence = nullptr;									// フェンス
	HANDLE m_fenceEvent = nullptr;											// フェンスイベント
	unsigned long long m_fenceValue = 0;									// フェンスの現在の値
	unsigned int m_frameIdx = 0;											//　現在フレームバッファのインデックス
	unsigned int m_windowWidth = 0;											// ウィンドウの横幅
	unsigned int m_windowHeight = 0;										// ウィンドウの縦幅
};

MY_LIB_NAMESPACE_END