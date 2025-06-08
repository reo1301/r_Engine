#pragma once
#include <d3d12.h>

MY_LIB_NAMESPACE_BEGIN

// 前方宣言
class CShader;

class CGraphicsPipelineStateWrapper
{
public:
	struct InitData
	{
		ID3D12Device* d3dDevice = nullptr;							// d3dデバイス
		CShader* vsShader = nullptr;								// 頂点シェーダー
		CShader* psShader = nullptr;								// ピクセルシェーダー
		D3D12_INPUT_ELEMENT_DESC* inputElementList = nullptr;		// 入力レイアウト
		unsigned int inputElementNum = 0;							// 入力レイアウト数
	};

public:
	CGraphicsPipelineStateWrapper();
	~CGraphicsPipelineStateWrapper();

	/// @brief 初期化
	/// @param _initData 初期化データ
	/// @return 成否
	bool Initialize(const InitData& _initData);

	/// @brief 終了
	void Finalize();

	/// @brief ルートシグネチャとパイプラインステートを設定する
	/// @param _commandList 設定するコマンドリスト
	/// @return 成否
	bool SetRootSignatureAndPipelineState(ID3D12GraphicsCommandList* _commandList);

private:
	/// @brief 開放
	void Release();

	/// @brief ルートシグネチャ作成
	/// @param _initData 初期化データ
	/// @return 成否
	bool CreateRootSignature(const InitData& _initData);

	/// @brief パイプラインステート作成
	/// @param _initData 初期化データ
	/// @return 成否
	bool CreatePipelineState(const InitData& _initData);

private:
	ID3D12RootSignature* m_rootSignature = nullptr;		// ルートシグネチャ
	ID3D12PipelineState* m_pipelineState = nullptr;		// パイプラインステート
};

MY_LIB_NAMESPACE_END
