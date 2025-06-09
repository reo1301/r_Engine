#pragma once
#include <d3d12.h>

MY_LIB_NAMESPACE_BEGIN

class ICShaderResource
{
public:
	ICShaderResource();
	virtual ~ICShaderResource();

	/// @brief ���\�[�X�쐬
	/// @param _size �T�C�Y
	/// @return ����
	bool CreateResource(UINT64 _size);

	/// @brief �o�b�t�@�R�s�[
	/// @param _dataPtr �f�[�^�ւ̃|�C���^
	/// @param _size �T�C�Y
	/// @return ����
	bool CopyBuffer(const void* _dataPtr, UINT64 _size);

private:
	/// @brief �J��
	void Release();

protected:
	ID3D12Resource* m_resource = nullptr;
};

MY_LIB_NAMESPACE_END
