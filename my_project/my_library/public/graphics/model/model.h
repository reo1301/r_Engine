#pragma once
#include "shader/shader.h"
#include "graphics/model/model_def.h"
#include "graphics/pipeline_state_wrapper.h"
#include "graphics/vertex_buffer.h"
#include <d3d12.h>

MY_LIB_NAMESPACE_BEGIN

class CModel
{
public:
	struct ModelInitData
	{
		CVertexBuffer::VertexData vertexData;		// 頂点データ
		CShader::ShaderInitData vsInitData;			// 頂点シェーダー初期化データ
		CShader::ShaderInitData psInitData;			// ピクセルシェーダー初期化データ
		MODEL_INPUT_LAYOUT_TYPE inputLayoutType = MODEL_INPUT_LAYOUT_TYPE_NONE;		// 入力レイアウトタイプ
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

private:
	CGraphicsPipelineStateWrapper m_pipelineState;		// パイプラインステート
	CVertexBuffer m_vertexBuffer;						// 頂点バッファ
	CShader m_vertexShader;		// 頂点シェーダー
	CShader m_pixelShader;		// ピクセルシェーダー
};

MY_LIB_NAMESPACE_END
