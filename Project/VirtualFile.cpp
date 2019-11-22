#include "stdafx.h"
#include "VirtualFile.h"

LPSTREAM CVirtualFile::CreateNewStream()
{
	return new CVirtualStream(this);
}

CVirtualFile* CVirtualFile::Create(CString encryptedFilePath)
{
	return new CVirtualFile(encryptedFilePath);
}

CVirtualFile::CVirtualFile(CString encryptedFilePath) : m_pBuffer(NULL), m_dwSize(0), m_nRefCount(1)
{
	m_EncryptedFilePath = encryptedFilePath;
	m_pCache = new CCache(encryptedFilePath);
	m_nCurrentPosition = 0;
	m_nCurrentChunkIndex = -1;
	m_DecryptedChunkSize = 0;
}

CVirtualFile::~CVirtualFile()
{
	if(NULL != m_pBuffer)
	{
		HeapFree(GetProcessHeap(), 0, m_pBuffer);
	}
}

void CVirtualFile::AddRef()
{
	InterlockedIncrement(&m_nRefCount);
}

void CVirtualFile::Release()
{
    if (0 == InterlockedDecrement(&m_nRefCount))
    {
        delete this;
    }
}

DWORD CVirtualFile::GetSize() const
{
	
    return m_dwSize;
}

void CVirtualFile::ReAlloc(ULARGE_INTEGER NewSize)
{
    CVirtualLock::Owner lock(m_lock);

    m_dwSize = NewSize.LowPart;

    if (NULL != m_pBuffer)
        m_pBuffer = HeapReAlloc(GetProcessHeap(), 0, m_pBuffer, m_dwSize);
    else
        m_pBuffer = HeapAlloc(GetProcessHeap(), 0, m_dwSize);
}
void CVirtualFile::Seek(DWORD currentPosition)
{
	CVirtualLock::Owner lock(m_lock);

	m_nCurrentPosition = currentPosition;
}

DWORD CVirtualFile::Read(DWORD nCurrentPosition, void* pv, ULONG cb)
{
    CVirtualLock::Owner lock(m_lock);

    DWORD nBytesToRead;
	std::vector<unsigned char> currentChunkBuffer;
	DWORD fileSize = m_pCache->GetDecryptedVideoSize();
	if(nCurrentPosition > m_pCache->GetDecryptedVideoSize())
	{
		nBytesToRead = 0;
	}
	else
	{
		DWORD restBytesToCopy = cb;
		DWORD totalReadBytes = 0;
		DWORD offsetInOutput = 0;
		DWORD mPosition = nCurrentPosition;
		while(restBytesToCopy > 0 && mPosition < m_pCache->GetDecryptedVideoSize())
		{
			// Find chunk number corresponding to current position
			DWORD requiredChunkIndex = mPosition / DecryptedChunkSize;
			DWORD decryptedChunkSize = 0;
			
			// Decrypt chunk corresponding to requiredChunkIndex
			m_nCurrentChunkIndex = requiredChunkIndex;
			std::vector<unsigned char> decryptedChunk;
			decryptedChunk = m_pCache->GetDecryptedChunk(requiredChunkIndex);
			decryptedChunkSize = decryptedChunk.size();
			//pDecryptedChunk = reinterpret_cast<unsigned char*>(decryptedChunk.data());
			m_DecryptedChunkSize = decryptedChunkSize;
			

			DWORD offsetInChunk = (DWORD)(mPosition - (m_nCurrentChunkIndex * DecryptedChunkSize));
			DWORD restInChunk = (DWORD)(m_DecryptedChunkSize - offsetInChunk);

			if(restInChunk < restBytesToCopy)
				nBytesToRead = restInChunk;
			else
				nBytesToRead = restBytesToCopy;

			
			for(int i = 0; i < nBytesToRead; ++i)
			{
				currentChunkBuffer.push_back(decryptedChunk[offsetInChunk + i]);
			}

			restBytesToCopy -= nBytesToRead;
			totalReadBytes += nBytesToRead;
			offsetInOutput += nBytesToRead;
			//m_nCurrentPosition += nBytesToRead;
			mPosition += nBytesToRead;  // We have new position and restBytesToRead from here
		}

		
	}
		nBytesToRead = 0;
		unsigned char* pDecryptedChunk;
		pDecryptedChunk = reinterpret_cast<unsigned char*>(currentChunkBuffer.data());

		if (nCurrentPosition + cb <= m_pCache->GetDecryptedVideoSize())
			nBytesToRead = cb;
		else
			nBytesToRead = currentChunkBuffer.size();
		// Copy data
		CopyMemory(pv, pDecryptedChunk, nBytesToRead);
    /*if (nCurrentPosition + cb <= m_dwSize)
        // nCurrentPosition ... nCurrentPosition + cb ... m_dwSize
        nBytesToRead = cb;
    else
        // nCurrentPosition ... m_dwSize ... nCurrentPosition + cb
        nBytesToRead = m_dwSize - nCurrentPosition;

    if (nBytesToRead > 0)
        CopyMemory(pv, (PBYTE)m_pBuffer + nCurrentPosition, nBytesToRead);
		*/
    return nBytesToRead;
}

DWORD CVirtualFile::Write(DWORD nCurrentPosition, const void* pv, ULONG cb)
{
    CVirtualLock::Owner lock(m_lock);

    if (nCurrentPosition + cb > m_dwSize)
        // Resize the buffer
    {
        // Recalculate size
        m_dwSize = nCurrentPosition + cb;

        if (NULL != m_pBuffer)
            m_pBuffer = HeapReAlloc(GetProcessHeap(), 0, m_pBuffer, m_dwSize);
        else
            m_pBuffer = HeapAlloc(GetProcessHeap(), 0, m_dwSize);
    }

    CopyMemory((PBYTE)m_pBuffer + nCurrentPosition, pv, cb);

    return cb;
}

DWORD CVirtualFile::GetDecryptedVideoSize()
{
	return m_pCache->GetDecryptedVideoSize();
}
CVirtualStream::CVirtualStream(CVirtualFile* pMyFile) : 
    m_nCurrentPosition(0), 
    m_nRefCount(1)
{
    m_pMyFile = pMyFile;
    m_pMyFile->AddRef();
}

CVirtualStream::~CVirtualStream()
{
    m_pMyFile->Release();
}

ULONG STDMETHODCALLTYPE CVirtualStream::AddRef()
{
    return InterlockedIncrement(&m_nRefCount);
}

ULONG STDMETHODCALLTYPE CVirtualStream::Release()
{
    ULONG nRefCount = InterlockedDecrement(&m_nRefCount);

    if (0 == nRefCount)
    {
        delete this;
    }

    return nRefCount;
}

HRESULT CVirtualStream::Seek(LARGE_INTEGER Distance, DWORD dwMoveMethod, ULARGE_INTEGER* NewPos)
{
    // Note: new position can be more than m_dwSize

    CVirtualLock::Owner lock(m_lock);
	/*if(m_nCurrentPosition > )
	{
		return S_OK;
	}*/
    LARGE_INTEGER new_pos = { 0 };

    if (STREAM_SEEK_SET == dwMoveMethod)
        new_pos.QuadPart = Distance.QuadPart;
    else if (STREAM_SEEK_CUR == dwMoveMethod)
        new_pos.QuadPart = Distance.QuadPart + m_nCurrentPosition;
    else if (STREAM_SEEK_END == dwMoveMethod)
		new_pos.QuadPart = m_pMyFile->GetDecryptedVideoSize() + Distance.QuadPart;

    m_nCurrentPosition = new_pos.LowPart;

	//m_pMyFile->Seek(m_nCurrentPosition);

    if (NewPos)
        NewPos->QuadPart = new_pos.QuadPart;

    return S_OK;
}

HRESULT CVirtualStream::SetSize(ULARGE_INTEGER newSize)
{
    CVirtualLock::Owner lock(m_lock);

    // SetSize (the current pointer should remain unchanged)
    m_pMyFile->ReAlloc(newSize);

    return S_OK;
}

HRESULT CVirtualStream::Stat(STATSTG* pstatstg, DWORD grfStatFlag)
{
    if (NULL == pstatstg)
        return E_INVALIDARG;

    ZeroMemory(pstatstg, sizeof(*pstatstg));
    pstatstg->type = STGTY_STREAM;
	pstatstg->cbSize.QuadPart = m_pMyFile->GetDecryptedVideoSize();
	
	
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CVirtualStream::QueryInterface(REFIID iid, void** ppObject)
{
    *ppObject = NULL;

    if (IsEqualIID(iid, IID_IUnknown))
        *ppObject = this;
    else if (IsEqualIID(iid, IID_IStream))
        *ppObject = (IStream*)this;

    if (NULL != *ppObject)
    {
        AddRef();
        return S_OK;
    }
    else
    {
        return E_NOINTERFACE;
    }
}

HRESULT STDMETHODCALLTYPE CVirtualStream::Read(void* pv, ULONG cb, ULONG* pcbRead)
{
    CVirtualLock::Owner lock(m_lock);

    DWORD nReadBytes = m_pMyFile->Read(m_nCurrentPosition, pv, cb);

    if (NULL != pcbRead)
        *pcbRead = nReadBytes;

    m_nCurrentPosition += nReadBytes;

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CVirtualStream::Write(const void* pv, ULONG cb, ULONG* pcbWritten)
{
   /* CVirtualLock::Owner lock(m_lock);

    DWORD nWrittenBytes = m_pMyFile->Write(m_nCurrentPosition, pv, cb);

    if (NULL != pcbWritten)
        *pcbWritten = nWrittenBytes;

    m_nCurrentPosition += nWrittenBytes;*/

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CVirtualStream::Clone(IStream** ppstm)
{
    CVirtualStream* pMyStream = new CVirtualStream(m_pMyFile);

    HRESULT hr = pMyStream->QueryInterface(IID_IStream, (void**)ppstm);

    pMyStream->Release();

    return hr;
}

HRESULT STDMETHODCALLTYPE CVirtualStream::CopyTo(IStream* pstm, ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead, ULARGE_INTEGER* pcbWritten)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CVirtualStream::Commit(DWORD grfCommitFlags)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CVirtualStream::Revert()
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CVirtualStream::LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CVirtualStream::UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
    return E_NOTIMPL;
}

CVirtualLock::Owner::Owner(CVirtualLock& lock) : 
    m_lock(lock)
{
    m_lock.Lock();
}

CVirtualLock::Owner::~Owner()
{
    m_lock.Unlock();
}

void CVirtualLock::Lock()
{
    EnterCriticalSection(&m_cs);
}

void CVirtualLock::Unlock()
{
    LeaveCriticalSection(&m_cs);
}

CVirtualLock::CVirtualLock()
{
    InitializeCriticalSection(&m_cs);
}

CVirtualLock::~CVirtualLock()
{
    DeleteCriticalSection(&m_cs);
}

void CreateCryptoStream(IStream** ppStream, CString encryptedFilePath)
{
    CVirtualFile* pMyFile = CVirtualFile::Create(encryptedFilePath);

    *ppStream = pMyFile->CreateNewStream();

    pMyFile->Release();

    //return S_OK;
}