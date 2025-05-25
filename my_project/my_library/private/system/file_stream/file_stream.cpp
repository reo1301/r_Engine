#include "system/file_stream/file_stream.h"
#include <fstream>

MY_LIB_NAMESPACE_BEGIN

CFileStream::CFileStream()
{
}

CFileStream::~CFileStream()
{
}

/// @brief ファイル操作
/// @param _fileStreamInfo ファイル操作情報
/// @return 成否
bool CFileStream::StreamFile(FileStreamInfo& _fileStreamInfo)
{
	bool isSuccess = true;
	if (_fileStreamInfo.filePath == nullptr)
	{
		printf("CFileStream::StreamFile ファイルパスが無効\n");
		isSuccess = false;
		return isSuccess;
	}

	bool isBinary = (_fileStreamInfo.type == FileStreamType_ReadBinary) || (_fileStreamInfo.type == FileStreamType_WriteBinary);
	switch (_fileStreamInfo.type)
	{
		// 読み込み
		case FileStreamType_Read:
		case FileStreamType_ReadBinary:
		{
			int openMode = std::ios::_Default_open_prot | std::ios::in;
			std::ifstream file(_fileStreamInfo.filePath, isBinary ? openMode | std::ios::binary : openMode);
			if (file.fail())
			{
				printf("CFileStream::StreamFile　ファイルオープンに失敗。filePath = %s, type = %d\n", _fileStreamInfo.filePath, _fileStreamInfo.type);
				isSuccess = false;
				break;
			}

			// ファイルサイズ確認
			file.seekg(0, std::ios_base::end);
			_fileStreamInfo.fileSize = file.tellg();

			if (_fileStreamInfo.fileSize == 0)
			{
				printf("CFileStream::StreamFile ファイルサイズが0です。filePath = %s, type = %d\n", _fileStreamInfo.filePath, _fileStreamInfo.type);
				isSuccess = false;
				break;
			}

			// 元の位置に戻す
			file.seekg(0, std::ios_base::beg);

			// メモリ確保
			_fileStreamInfo.fileBuffer = malloc(_fileStreamInfo.fileSize);

			// ファイル読み込み
			file.read(static_cast<char*>(_fileStreamInfo.fileBuffer), _fileStreamInfo.fileSize);
		}
		break;

		// 書き込み
		case FileStreamType_Write:
		case FileStreamType_WriteBinary:
		{
			if (_fileStreamInfo.fileSize == 0)
			{
				printf("CFileStream::StreamFile ファイルサイズが0です。filePath = %s, type = %d\n", _fileStreamInfo.filePath, _fileStreamInfo.type);
				isSuccess = false;
				break;
			}

			int openMode = std::ios::out;
			std::ofstream file(_fileStreamInfo.filePath, isBinary ? openMode | std::ios::binary : openMode);
			if (file.fail())
			{
				printf("CFileStream::StreamFile　ファイルオープンに失敗。filePath = %s, type = %d\n", _fileStreamInfo.filePath, _fileStreamInfo.type);
				isSuccess = false;
				break;
			}

			// ファイル書き込み
			file.write(static_cast<const char*>(_fileStreamInfo.fileBuffer), _fileStreamInfo.fileSize);
		}
		break;

		default:
		{
			printf("ファイル操作タイプが無効。type = %d\n", _fileStreamInfo.type);
			isSuccess = false;
		}
		break;
	}

	return isSuccess;
}

MY_LIB_NAMESPACE_END
