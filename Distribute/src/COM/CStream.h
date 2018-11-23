#pragma once
#define m_align32 32L
#define MEMALIGN_32(cb) cb % m_align32 != 0 ? ((cb / m_align32) + 1) * m_align32 : cb
#include "CSession.h"
class ATL_NO_VTABLE  CStream: 
	public CComObjectRoot,
	public CComCoClass<CStream>,
	public IStream
{

public:	
	HRESULT FinalConstruct();
	void FinalRelease();
	DECLARE_NO_REGISTRY()
	DECLARE_NOT_AGGREGATABLE(CStream)
	

	BEGIN_COM_MAP(CStream)
		COM_INTERFACE_ENTRY(IStream)
	END_COM_MAP()
	//	Implementation of ISequentialStream
	STDMETHOD(Read)(void*, ULONG, ULONG *);
	STDMETHOD(Write) (const void*, ULONG, ULONG *);
	// Implementation of IStream	
	STDMETHOD(Seek)(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition);
	STDMETHOD(SetSize)(ULARGE_INTEGER libNewSize);
	STDMETHOD(CopyTo)(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten);
	STDMETHOD(Commit)(DWORD grfCommitFlags);
	STDMETHOD(Revert)(void);
	STDMETHOD(LockRegion)(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
	STDMETHOD(UnlockRegion)(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
	STDMETHOD(Stat)(STATSTG *pstatstg, DWORD grfStatFlag);
	STDMETHOD(Clone)(IStream **ppstm);

	void Clear();
private:
	//bool m_WasAssignedArray;
	//ULONG       m_iReadPos;     // Current index position for reading from the buffer.
	ULARGE_INTEGER      m_iWritePos;    // Current index position for writing to the buffer.
	
	ULARGE_INTEGER		m_ulSize;		// length of the buffer
	ULARGE_INTEGER		m_ulLength;     // Total assigned buffer length.
	//ULONG		m_refCount;
	//bool		m_DidAlloc;
	
	std::vector<BYTE> m_buf;
	

};
OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(GUID_NULL, CStream)