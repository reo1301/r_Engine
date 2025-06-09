#include "graphics/shader_resource_interface.h"
#include "graphics/graphics_engine.h"

ICShaderResource::ICShaderResource()
{

}

ICShaderResource::~ICShaderResource()
{
	Release();
}

/// @brief ���\�[�X�쐬
/// @param _size �T�C�Y
/// @return ����
bool ICShaderResource::CreateResource(UINT64 _size)
{
	if (m_resource != nullptr)
	{
		printf("ICShaderResource::CreateResource ���\�[�X�͂��łɐ����ς݂ł�\n");
		return true;
	}

	ID3D12Device* d3dDevice = CGraphicsEngine::GetInstance().GetD3dDevice();
	if (d3dDevice == nullptr)
	{
		printf("ICShaderResource::CreateResource d3dDevice��null�ł�\n");
		return false;
	}

	// �q�[�v�v���p�e�B
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.CreationNodeMask = 1;
	heapProp.VisibleNodeMask = 1;

	// ���\�[�X���
	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = _size;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	HRESULT result = d3dDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_resource));
	if (FAILED(result))
	{
		printf("ICShaderResource::CreateResource ���\�[�X�̍쐬�Ɏ��s�B\n");
		return false;
	}

	return true;
}

/// @brief �o�b�t�@�R�s�[
/// @param _dataPtr �f�[�^�ւ̃|�C���^
/// @param _size �T�C�Y
/// @return ����
bool ICShaderResource::CopyBuffer(const void* _dataPtr, UINT64 _size)
{
	if (m_resource == nullptr)
	{
		printf("ICShaderResource::CopyBuffer m_resource��null�ł�\n");
		return false;
	}

	// �o�b�t�@�R�s�[
	void* bufferPtr = nullptr;
	HRESULT result = m_resource->Map(0, nullptr, reinterpret_cast<void**>(&bufferPtr));
	if (FAILED(result))
	{
		printf("ICShaderResource::CopyBuffer �o�b�t�@�̃}�b�v�Ɏ��s\n");
		return false;
	}

	// �������R�s�[
	memcpy_s(bufferPtr, _size, _dataPtr, _size);
	m_resource->Unmap(0, nullptr);

	return true;

}

/// @brief �J��
void ICShaderResource::Release()
{
	if (m_resource != nullptr)
	{
		m_resource->Release();
		m_resource = nullptr;
	}
}
