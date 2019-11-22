#include "stdafx.h"
#include "Cache.h"
#include <fstream>
#include "EncryptData.h"

DWORD CCache::GetDecryptedVideoSize()
{
	if(m_DecryptedVideoSize != 0)
	{
		// size already computed
		return m_DecryptedVideoSize;
	}
	else
	{
		// Compute decrypted video size
		std::ifstream encryptedFile;

		encryptedFile.open((LPCTSTR)m_EncryptedVideoFilePath, std::ios::binary || std::ios::in);
		if(encryptedFile.fail())
			return 0;

		DWORD begin = encryptedFile.tellg();
		encryptedFile.seekg(0, encryptedFile.end);
		DWORD end   = encryptedFile.tellg();
		encryptedFile.seekg(0, encryptedFile.beg);

		DWORD encryptedFileSize = end - begin;
		DWORD approximateChunks = encryptedFileSize / EncryptedChunkSize;
		DWORD DecryptedSize = approximateChunks * DecryptedChunkSize;; // This is approximate decrypted file size

		if(!((approximateChunks * EncryptedChunkSize) == encryptedFileSize))
		{
			 DWORD extraBytes = encryptedFileSize - (approximateChunks * EncryptedChunkSize) - 16; // 16 byte header for last chunk
			 DecryptedSize += extraBytes;
			 approximateChunks += 1;
		}
		
		// fill member variables
		m_EncryptedVideoSize = encryptedFileSize;
		m_DecryptedVideoSize = DecryptedSize;
		m_nEncryptedChunks = approximateChunks;
	}
	return m_DecryptedVideoSize;
}

// Get decrypted chunk
std::vector<unsigned char> CCache::GetDecryptedChunk(DWORD chunkIndex)
{
	if(m_CurrentChunkIndex == chunkIndex)
	{
		return m_DecryptedChunk;
	}
	else
	{
		// Decrypt new chunk and return the decrypted chunk

		// Get chunk
		std::ifstream encryptedVideo;
		encryptedVideo.open((LPCTSTR)m_EncryptedVideoFilePath, std::ios::binary | std::ios::in);
		if(encryptedVideo.fail())
		{
		}
		else
		{
		unsigned int seekPosition =  chunkIndex * EncryptedChunkSize;
		encryptedVideo.seekg(seekPosition); // Starting of chunk
		char buffer[EncryptedChunkSize];
		encryptedVideo.read(buffer, EncryptedChunkSize);
		int readBytes = encryptedVideo.gcount();
		encryptedVideo.close(); // close the file handle
		EncryptData *encryptor = new EncryptAES();
		m_DecryptedChunk = encryptor->Decrypt(buffer, readBytes);

		m_CurrentChunkIndex = chunkIndex;
		}
		return m_DecryptedChunk;
	}
}