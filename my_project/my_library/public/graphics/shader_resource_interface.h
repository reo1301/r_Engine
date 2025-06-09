#pragma once
#include <d3d12.h>

MY_LIB_NAMESPACE_BEGIN

class ICShaderResource
{
public:
	ICShaderResource();
	virtual ~ICShaderResource();

	/// @brief リソース作成
	/// @param _size サイズ
	/// @return 成否
	bool CreateResource(UINT64 _size);

	/// @brief バッファコピー
	/// @param _dataPtr データへのポインタ
	/// @param _size サイズ
	/// @return 成否
	bool CopyBuffer(const void* _dataPtr, UINT64 _size);

private:
	/// @brief 開放
	void Release();

protected:
	ID3D12Resource* m_resource = nullptr;
};

MY_LIB_NAMESPACE_END
