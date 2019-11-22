#include <string>
#include <vector>
using namespace std;
const unsigned int EncryptedChunkSize = 50016;
const unsigned int DecryptedChunkSize = 50000;

class CCache
{
	CString m_EncryptedVideoFilePath;

	DWORD m_EncryptedVideoSize;

	DWORD m_DecryptedVideoSize;

	DWORD m_nEncryptedChunks;

	DWORD m_CurrentChunkIndex;

	std::vector<unsigned char> m_DecryptedChunk;
public :
	CCache();

	CCache(CString encryptedVideoFilePath)
	{
		m_EncryptedVideoFilePath = encryptedVideoFilePath;
		m_EncryptedVideoSize = 0;
		m_DecryptedVideoSize = 0;
	}

	

	DWORD GetDecryptedVideoSize();
	std::vector<unsigned char> GetDecryptedChunk(DWORD chunkIndex);
};