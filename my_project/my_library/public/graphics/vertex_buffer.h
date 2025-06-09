#pragma once
#include <d3d12.h>

MY_LIB_NAMESPACE_BEGIN

class CVertexBuffer
{
public:
	struct VertexData
	{
		const float* vertexDataPtr = nullptr;
		unsigned long long vertexDataSize = 0;
		unsigned long long vertexDataStride = 0;
	};

	struct InitData
	{
		ID3D12Device* d3dDevice = nullptr;		// d3dデバイス
		VertexData vertexData;					// 頂点データ
	};

public:
	CVertexBuffer();
	~CVertexBuffer();

	/// @brief 初期化
	/// @param _initData 初期化データ
	/// @return 成否
	bool Initialize(const InitData& _initData);

	/// @brief 終了
	void Finalize();

	/// @brief 頂点バッファビューを取得
	/// @return 頂点バッファビュー
	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView();

	/// @brief 頂点数を取得
	/// @return 頂点数
	unsigned int GetVertexCount();

private:
	/// @brief 開放
	void Release();

	/// @brief リソース作成
	/// @param _initData 初期化データ
	/// @return 成否
	bool CreateResource(const InitData& _initData);

	/// @brief バッファをコピー
	/// @param _vertexData 頂点データ
	/// @return 成否
	bool CopyBuffer(const VertexData& _vertexData);

private:
	ID3D12Resource* m_vertexBuffer = nullptr;			// 頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;		// 頂点バッファビュー
	unsigned int m_vertexCount = 0;						// 頂点数
};

MY_LIB_NAMESPACE_END
