#include <ObjIdl.h>
#include "Cache.h"
void CreateCryptoStream(IStream** ppStream, CString encryptedFilePath);

class CVirtualLock
{
private:
	CRITICAL_SECTION m_cs;

public:
	class Owner
	{
	private:
		CVirtualLock& m_lock;

	public:
		explicit Owner(CVirtualLock& lock);
		~Owner();
	};

	friend class Owner;

private:
	void Lock();
	void Unlock();

public:
	CVirtualLock();
	~CVirtualLock();
};

class CVirtualFile
{
private:
    // Reference count
    LONG m_nRefCount;

    // Memory buffer that contains data of the file
    PVOID m_pBuffer;

    // Size of the buffer
    DWORD m_dwSize;

    // Lock
    CVirtualLock m_lock;

	// EncryptedFilePath
	CString m_EncryptedFilePath;

	CCache *m_pCache;

	// Current position being read
	DWORD m_nCurrentPosition;

	// Current chunk used for reading
	unsigned char* m_CurrentChunkBuffer;

	// Current chunk index
	DWORD m_nCurrentChunkIndex;

	// Current decrypted chunk size
	DWORD m_DecryptedChunkSize;

public:
	static CVirtualFile* Create(CString encryptedFilePath);

	LPSTREAM CreateNewStream();

private:
	CVirtualFile(CString encryptedFilePath);
	~CVirtualFile();

public:
	void AddRef();
    void Release();

    DWORD GetSize() const;

	DWORD GetDecryptedVideoSize();

    void ReAlloc(ULARGE_INTEGER NewSize);

    DWORD Read(DWORD nCurrentPosition, void* pv, ULONG cb);
    DWORD Write(DWORD nCurrentPosition, const void* pv, ULONG cb);

	void Seek(DWORD currentPosition);
};

class CVirtualStream : public IStream
{
private:
    LONG m_nRefCount;

    CVirtualFile* m_pMyFile;

    DWORD m_nCurrentPosition;

    CVirtualLock m_lock;

public:
    explicit CVirtualStream(CVirtualFile* pMyFile);
    ~CVirtualStream();

    // IUnknown
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

    // ISequentialStream
    virtual HRESULT STDMETHODCALLTYPE Read(void* pv, ULONG cb, ULONG* pcbRead);
    virtual HRESULT STDMETHODCALLTYPE Write(const void* pv, ULONG cb, ULONG* pcbWritten);

    // IStream
    virtual HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition);
    virtual HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize);
    virtual HRESULT STDMETHODCALLTYPE CopyTo(IStream* pstm, ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead, ULARGE_INTEGER* pcbWritten);
    virtual HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags);
    virtual HRESULT STDMETHODCALLTYPE Revert();
    virtual HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
    virtual HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
    virtual HRESULT STDMETHODCALLTYPE Stat(STATSTG* pstatstg, DWORD grfStatFlag);
    virtual HRESULT STDMETHODCALLTYPE Clone(IStream** ppstm);	
};