#include "graphics/vertex_buffer.h"
#include "graphics/graphics_engine.h"

MY_LIB_NAMESPACE_BEGIN

CVertexBuffer::CVertexBuffer()
	: ICShaderResource()
{

}

CVertexBuffer::~CVertexBuffer()
{

}

/// @brief 初期化
/// @param _initData 初期化データ
/// @return 成否
bool CVertexBuffer::Initialize(const InitData& _initData)
{
	if ((_initData.vertexDataPtr == nullptr) || (_initData.vertexDataSize == 0) || (_initData.vertexDataStride == 0))
	{
		printf("CVertexBuffer::Initialize 頂点データが無効\n");
		return false;
	}

	// リソース作成
	if (!CreateResource(_initData.vertexDataSize))
	{
		printf("CVertexBuffer::Initialize 頂点バッファのリソース作成に失敗\n");
		return false;
	}

	// バッファコピー
	if (!CopyBuffer(_initData.vertexDataPtr, _initData.vertexDataSize))
	{
		printf("CVertexBuffer::Initialize バッファのコピーに失敗\n");
		return false;
	}

	m_vertexBufferView.BufferLocation = m_resource->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = static_cast<UINT>(_initData.vertexDataSize);
	m_vertexBufferView.StrideInBytes = static_cast<UINT>(_initData.vertexDataStride);

	m_vertexCount = static_cast<unsigned int>(_initData.vertexDataSize / _initData.vertexDataStride);

	return true;
}

/// @brief 頂点バッファビューを取得
/// @return 頂点バッファビュー
const D3D12_VERTEX_BUFFER_VIEW& CVertexBuffer::GetVertexBufferView()
{
	return m_vertexBufferView;
}

/// @brief 頂点数を取得
/// @return 頂点数
unsigned int CVertexBuffer::GetVertexCount()
{
	return m_vertexCount;
}

MY_LIB_NAMESPACE_END
