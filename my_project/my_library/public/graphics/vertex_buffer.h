#pragma once
#include "shader_resource_interface.h"

MY_LIB_NAMESPACE_BEGIN

class CVertexBuffer : public ICShaderResource
{
public:
	struct InitData
	{
		const float* vertexDataPtr = nullptr;
		unsigned long long vertexDataSize = 0;
		unsigned long long vertexDataStride = 0;
	};

public:
	CVertexBuffer();
	~CVertexBuffer();

	/// @brief 初期化
	/// @param _initData 初期化データ
	/// @return 成否
	bool Initialize(const InitData& _initData);

	/// @brief 頂点バッファビューを取得
	/// @return 頂点バッファビュー
	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView();

	/// @brief 頂点数を取得
	/// @return 頂点数
	unsigned int GetVertexCount();

private:
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;		// 頂点バッファビュー
	unsigned int m_vertexCount = 0;						// 頂点数
};

MY_LIB_NAMESPACE_END
