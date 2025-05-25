#pragma once

MY_LIB_NAMESPACE_BEGIN

class CFileStream
{
public:
	enum FileStreamType
	{
		FileStreamType_None = 0,		// 無効
		FileStreamType_Read,			// 読み込み
		FileStreamType_ReadBinary,		// 読み込み(バイナリ)
		FileStreamType_Write,			// 書き込み
		FileStreamType_WriteBinary,		// 書き込み(バイナリ)

		FileStreamType_Num
	};

	struct FileStreamInfo
	{
		const char* filePath = nullptr;					// ファイルパス
		void* fileBuffer = nullptr;						// ファイルバッファ
		unsigned long long fileSize = 0;				// ファイルサイズ
		FileStreamType type = FileStreamType_None;		// 操作タイプ
	};

public:
	CFileStream();
	~CFileStream();

	/// @brief ファイル操作
	/// @param _fileStreamInfo ファイル操作情報
	/// @return 成否
	bool StreamFile(FileStreamInfo& _fileStreamInfo);

private:
};

MY_LIB_NAMESPACE_END
