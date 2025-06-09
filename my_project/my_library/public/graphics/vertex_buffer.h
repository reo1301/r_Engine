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
		ID3D12Device* d3dDevice = nullptr;		// d3d�f�o�C�X
		VertexData vertexData;					// ���_�f�[�^
	};

public:
	CVertexBuffer();
	~CVertexBuffer();

	/// @brief ������
	/// @param _initData �������f�[�^
	/// @return ����
	bool Initialize(const InitData& _initData);

	/// @brief �I��
	void Finalize();

	/// @brief ���_�o�b�t�@�r���[���擾
	/// @return ���_�o�b�t�@�r���[
	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView();

	/// @brief ���_�����擾
	/// @return ���_��
	unsigned int GetVertexCount();

private:
	/// @brief �J��
	void Release();

	/// @brief ���\�[�X�쐬
	/// @param _initData �������f�[�^
	/// @return ����
	bool CreateResource(const InitData& _initData);

	/// @brief �o�b�t�@���R�s�[
	/// @param _vertexData ���_�f�[�^
	/// @return ����
	bool CopyBuffer(const VertexData& _vertexData);

private:
	ID3D12Resource* m_vertexBuffer = nullptr;			// ���_�o�b�t�@
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;		// ���_�o�b�t�@�r���[
	unsigned int m_vertexCount = 0;						// ���_��
};

MY_LIB_NAMESPACE_END
