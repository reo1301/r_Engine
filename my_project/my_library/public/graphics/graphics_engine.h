#pragma once

//インクルード
//コピー禁止
#include "../noncopyable.h"

//D3D系
#include <d3d12.h>
#include <dxgi1_4.h>

// D3Dのラッパー
#include "graphics/graphics_command_wrapper.h"
#include "graphics/graphics_descriptor_heap_wrapper.h"

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

	/// @brief スワップチェーンを作成
	void CreateSwapChain();

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
	CGraphicsCommandWrapper m_commandWrapper;						// D3Dコマンドラッパー
	CGraphicsDescriptorHeapWrapper m_descriptorHeap;				// RTV用ディスクリプタヒープ
	IDXGIFactory4* m_dxgiFactory = nullptr;							// DXGIファクトリ
	ID3D12Device* m_d3d12Device = nullptr;							// D3D12デバイス
	IDXGISwapChain3* m_dxgiSwapChain = nullptr;						// スワップチェーン
	ID3D12Resource* m_renderTarget[RTV_NUM] = {};					// レンダーターゲット
	ID3D12Fence* m_d3d12Fence = nullptr;							// フェンス
	HANDLE m_fenceEvent = nullptr;									// フェンスイベント
	unsigned long long m_fenceValue = 0;							// フェンスの現在の値
	unsigned int m_frameIdx = 0;									//　現在フレームバッファのインデックス
	unsigned int m_windowWidth = 0;									// ウィンドウの横幅
	unsigned int m_windowHeight = 0;								// ウィンドウの縦幅
};

MY_LIB_NAMESPACE_END