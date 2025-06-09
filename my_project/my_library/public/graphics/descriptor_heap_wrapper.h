#pragma once
#include <d3d12.h>

MY_LIB_NAMESPACE_BEGIN

class CGraphicsDescriptorHeapWrapper
{
public:
	struct InitData
	{
		D3D12_DESCRIPTOR_HEAP_TYPE heapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;		// ヒープタイプ
		unsigned int numDescriptor = 0;			// ディスクリプタ数
	};

public:
	CGraphicsDescriptorHeapWrapper();
	~CGraphicsDescriptorHeapWrapper();

	/// @brief 初期化
	/// @param _initData 初期化データ
	/// @return 成否
	bool Initialize(const InitData& _initData);

	/// @brief 終了
	void Finalize();

	/// @brief ディスクリプタヒープの取得
	/// @return ディスクリプタヒープ
	ID3D12DescriptorHeap* GetDescriptorHeap();

private:
	/// @brief 開放
	void Release();

	/// @brief ディスクリプタヒープ
	/// @param _initData 初期化データ
	/// @return 成否
	bool CreateDescriptorHeap(const InitData& _initData);

private:
	ID3D12DescriptorHeap* m_descriptorHeap = nullptr;		// ディスクリプタヒープ

};

MY_LIB_NAMESPACE_END
