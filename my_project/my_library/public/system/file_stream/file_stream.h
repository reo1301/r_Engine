#pragma once

MY_LIB_NAMESPACE_BEGIN

class CFileStream
{
public:
	enum FileStreamType
	{
		FileStreamType_None = 0,		// ����
		FileStreamType_Read,			// �ǂݍ���
		FileStreamType_ReadBinary,		// �ǂݍ���(�o�C�i��)
		FileStreamType_Write,			// ��������
		FileStreamType_WriteBinary,		// ��������(�o�C�i��)

		FileStreamType_Num
	};

	struct FileStreamInfo
	{
		const char* filePath = nullptr;					// �t�@�C���p�X
		void* fileBuffer = nullptr;						// �t�@�C���o�b�t�@
		unsigned long long fileSize = 0;				// �t�@�C���T�C�Y
		FileStreamType type = FileStreamType_None;		// ����^�C�v
	};

public:
	CFileStream();
	~CFileStream();

	/// @brief �t�@�C������
	/// @param _fileStreamInfo �t�@�C��������
	/// @return ����
	bool StreamFile(FileStreamInfo& _fileStreamInfo);

private:
};

MY_LIB_NAMESPACE_END
