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

/// @brief ������
/// @param _initData �������f�[�^
/// @return ����
bool CVertexBuffer::Initialize(const InitData& _initData)
{
	if ((_initData.vertexDataPtr == nullptr) || (_initData.vertexDataSize == 0) || (_initData.vertexDataStride == 0))
	{
		printf("CVertexBuffer::Initialize ���_�f�[�^������\n");
		return false;
	}

	// ���\�[�X�쐬
	if (!CreateResource(_initData.vertexDataSize))
	{
		printf("CVertexBuffer::Initialize ���_�o�b�t�@�̃��\�[�X�쐬�Ɏ��s\n");
		return false;
	}

	// �o�b�t�@�R�s�[
	if (!CopyBuffer(_initData.vertexDataPtr, _initData.vertexDataSize))
	{
		printf("CVertexBuffer::Initialize �o�b�t�@�̃R�s�[�Ɏ��s\n");
		return false;
	}

	m_vertexBufferView.BufferLocation = m_resource->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = static_cast<UINT>(_initData.vertexDataSize);
	m_vertexBufferView.StrideInBytes = static_cast<UINT>(_initData.vertexDataStride);

	m_vertexCount = static_cast<unsigned int>(_initData.vertexDataSize / _initData.vertexDataStride);

	return true;
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

MY_LIB_NAMESPACE_END
