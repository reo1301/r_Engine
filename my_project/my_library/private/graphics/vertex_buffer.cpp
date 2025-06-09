#include "graphics/vertex_buffer.h"

MY_LIB_NAMESPACE_BEGIN

CVertexBuffer::CVertexBuffer()
{

}

CVertexBuffer::~CVertexBuffer()
{

}


/// @brief ������
/// @param _initData �������f�[�^
/// @return ����
bool CVertexBuffer::Initialize(const InitData& _initData)
{
	const VertexData& vertexData = _initData.vertexData;
	if ((vertexData.vertexDataPtr == nullptr) || (vertexData.vertexDataSize == 0) || (vertexData.vertexDataStride == 0))
	{
		printf("CVertexBuffer::Initialize ���_�f�[�^������\n");
		return false;
	}

	// ���\�[�X�쐬
	if (!CreateResource(_initData))
	{
		printf("CVertexBuffer::Initialize ���_�o�b�t�@�̃��\�[�X�쐬�Ɏ��s\n");
		return false;
	}

	// �o�b�t�@�R�s�[
	if (!CopyBuffer(_initData.vertexData))
	{
		printf("CVertexBuffer::Initialize �o�b�t�@�̃R�s�[�Ɏ��s\n");
		return false;
	}

	m_vertexCount = static_cast<unsigned int>(_initData.vertexData.vertexDataSize / _initData.vertexData.vertexDataStride);

	return true;
}

/// @brief �I��
void CVertexBuffer::Finalize()
{
	Release();
}

/// @brief ���_�o�b�t�@�r���[���擾
/// @return ���_�o�b�t�@�r���[
const D3D12_VERTEX_BUFFER_VIEW& CVertexBuffer::GetVertexBufferView()
{
	return m_vertexBufferView;
}

/// @brief ���_�����擾
/// @return ���_��
unsigned int CVertexBuffer::GetVertexCount()
{
	return m_vertexCount;
}

/// @brief �J��
void CVertexBuffer::Release()
{
	if (m_vertexBuffer != nullptr)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
}

/// @brief ���\�[�X�쐬
/// @param _initData �������f�[�^
/// @return ����
bool CVertexBuffer::CreateResource(const InitData& _initData)
{
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
	resourceDesc.Width = _initData.vertexData.vertexDataSize;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	HRESULT result = _initData.d3dDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_vertexBuffer));
	if (FAILED(result))
	{
		printf("CModel::Initialize ���_�o�b�t�@�̍쐬�Ɏ��s�B\n");
		return false;
	}

	return true;
}

/// @brief �o�b�t�@���R�s�[
/// @param _vertexData ���_�f�[�^
/// @return ����
bool CVertexBuffer::CopyBuffer(const VertexData& _vertexData)
{
	if (m_vertexBuffer == nullptr)
	{
		printf("CVertexBuffer::CopyBuffer m_vertexBuffer��null�ł�\n");
		return false;
	}

	// ���_�o�b�t�@�ɃR�s�[
	void* vertexPtr = nullptr;
	HRESULT result = m_vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vertexPtr));
	if (FAILED(result))
	{
		printf("CVertexBuffer::CopyBuffer ���_�o�b�t�@�̃}�b�v�Ɏ��s\n");
		return false;
	}
	memcpy_s(vertexPtr, _vertexData.vertexDataSize, _vertexData.vertexDataPtr, _vertexData.vertexDataSize);
	m_vertexBuffer->Unmap(0, nullptr);

	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = static_cast<UINT>(_vertexData.vertexDataSize);
	m_vertexBufferView.StrideInBytes = static_cast<UINT>(_vertexData.vertexDataStride);

	return true;
}

MY_LIB_NAMESPACE_END
