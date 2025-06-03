#pragma once
#include "shader/shader.h"
#include "graphics/model/model_def.h"
#include <d3d12.h>

MY_LIB_NAMESPACE_BEGIN

class CModel
{
public:
	struct VertexData
	{
		const float* vertexDataPtr = nullptr;
		unsigned long long vertexDataSize = 0;
		unsigned long long vertexDataStride = 0;
	};

	struct ModelInitData
	{
		VertexData vertexData;
		CShader::ShaderInitData vsInitData;
		CShader::ShaderInitData psInitData;
		MODEL_INPUT_LAYOUT_TYPE inputLayoutType = MODEL_INPUT_LAYOUT_TYPE_NONE;
	};

public:
	CModel();
	~CModel();

	/// @brief 開放
	void Release();

	/// @brief 初期化
	/// @param _initData 初期化データ
	/// @return 成否
	bool Initialize(const ModelInitData& _initData);

	/// @brief 更新
	void Update();

	/// @brief 描画
	void Draw();

	/// @brief 後更新
	void PostUpdate();

private:
	/// @brief シェーダーのロード
	/// @param _vsInitData 頂点シェーダー初期化データ
	/// @param _psInitData ピクセルシェーダー初期化データ
	/// @return 成否
	bool LoadShader(const CShader::ShaderInitData& _vsInitData, const CShader::ShaderInitData& _psInitData);

	/// @brief ルートシグネチャを作成
	/// @return 成否
	bool CreateRootSignature();

	/// @brief ディスクリプタヒープを作成
	/// @return 成否
	bool CreateDescriptorHeap();

private:
	ID3D12RootSignature* m_rootSignature = nullptr;			// ルートシグネチャ
	ID3D12DescriptorHeap* m_descriptorHeap = nullptr;		// ディスクリプタヒープ
	ID3D12PipelineState* m_pipelineState = nullptr;			// パイプラインステート
	ID3D12Resource* m_vertexBuffer = nullptr;				// 頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;			// 頂点バッファビュー
	CShader m_vertexShader;		// 頂点シェーダー
	CShader m_pixelShader;		// ピクセルシェーダー
};

MY_LIB_NAMESPACE_END
