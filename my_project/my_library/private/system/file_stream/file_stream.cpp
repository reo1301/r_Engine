#include "system/file_stream/file_stream.h"
#include <fstream>

MY_LIB_NAMESPACE_BEGIN

CFileStream::CFileStream()
{
}

CFileStream::~CFileStream()
{
}

/// @brief �t�@�C������
/// @param _fileStreamInfo �t�@�C��������
/// @return ����
bool CFileStream::StreamFile(FileStreamInfo& _fileStreamInfo)
{
	bool isSuccess = true;
	if (_fileStreamInfo.filePath == nullptr)
	{
		printf("CFileStream::StreamFile �t�@�C���p�X������\n");
		isSuccess = false;
		return isSuccess;
	}

	bool isBinary = (_fileStreamInfo.type == FileStreamType_ReadBinary) || (_fileStreamInfo.type == FileStreamType_WriteBinary);
	switch (_fileStreamInfo.type)
	{
		// �ǂݍ���
		case FileStreamType_Read:
		case FileStreamType_ReadBinary:
		{
			int openMode = std::ios::_Default_open_prot | std::ios::in;
			std::ifstream file(_fileStreamInfo.filePath, isBinary ? openMode | std::ios::binary : openMode);
			if (file.fail())
			{
				printf("CFileStream::StreamFile�@�t�@�C���I�[�v���Ɏ��s�BfilePath = %s, type = %d\n", _fileStreamInfo.filePath, _fileStreamInfo.type);
				isSuccess = false;
				break;
			}

			// �t�@�C���T�C�Y�m�F
			file.seekg(0, std::ios_base::end);
			_fileStreamInfo.fileSize = file.tellg();

			if (_fileStreamInfo.fileSize == 0)
			{
				printf("CFileStream::StreamFile �t�@�C���T�C�Y��0�ł��BfilePath = %s, type = %d\n", _fileStreamInfo.filePath, _fileStreamInfo.type);
				isSuccess = false;
				break;
			}

			// ���̈ʒu�ɖ߂�
			file.seekg(0, std::ios_base::beg);

			// �������m��
			_fileStreamInfo.fileBuffer = malloc(_fileStreamInfo.fileSize);

			// �t�@�C���ǂݍ���
			file.read(static_cast<char*>(_fileStreamInfo.fileBuffer), _fileStreamInfo.fileSize);
		}
		break;

		// ��������
		case FileStreamType_Write:
		case FileStreamType_WriteBinary:
		{
			if (_fileStreamInfo.fileSize == 0)
			{
				printf("CFileStream::StreamFile �t�@�C���T�C�Y��0�ł��BfilePath = %s, type = %d\n", _fileStreamInfo.filePath, _fileStreamInfo.type);
				isSuccess = false;
				break;
			}

			int openMode = std::ios::out;
			std::ofstream file(_fileStreamInfo.filePath, isBinary ? openMode | std::ios::binary : openMode);
			if (file.fail())
			{
				printf("CFileStream::StreamFile�@�t�@�C���I�[�v���Ɏ��s�BfilePath = %s, type = %d\n", _fileStreamInfo.filePath, _fileStreamInfo.type);
				isSuccess = false;
				break;
			}

			// �t�@�C����������
			file.write(static_cast<const char*>(_fileStreamInfo.fileBuffer), _fileStreamInfo.fileSize);
		}
		break;

		default:
		{
			printf("�t�@�C������^�C�v�������Btype = %d\n", _fileStreamInfo.type);
			isSuccess = false;
		}
		break;
	}

	return isSuccess;
}

MY_LIB_NAMESPACE_END
