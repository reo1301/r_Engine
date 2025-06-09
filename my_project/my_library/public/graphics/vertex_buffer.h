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

	/// @brief ������
	/// @param _initData �������f�[�^
	/// @return ����
	bool Initialize(const InitData& _initData);

	/// @brief ���_�o�b�t�@�r���[���擾
	/// @return ���_�o�b�t�@�r���[
	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView();

	/// @brief ���_�����擾
	/// @return ���_��
	unsigned int GetVertexCount();

private:
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;		// ���_�o�b�t�@�r���[
	unsigned int m_vertexCount = 0;						// ���_��
};

MY_LIB_NAMESPACE_END
