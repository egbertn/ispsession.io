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
	HRESULT FinalConstruct() noexcept;
	void FinalRelease() noexcept;
	DECLARE_NO_REGISTRY()
	DECLARE_NOT_AGGREGATABLE(CStream)
	

	BEGIN_COM_MAP(CStream)
		COM_INTERFACE_ENTRY(IStream)
	END_COM_MAP()
	//	Implementation of ISequentialStream
	STDMETHOD(Read)(void*, ULONG, ULONG *) noexcept;
	STDMETHOD(Write) (const void*, ULONG, ULONG *) noexcept;
	// Implementation of IStream	
	STDMETHOD(Seek)(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition) noexcept;
	STDMETHOD(SetSize)(ULARGE_INTEGER libNewSize) noexcept;
	STDMETHOD(CopyTo)(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten) noexcept;
	STDMETHOD(Commit)(DWORD grfCommitFlags) noexcept;
	STDMETHOD(Revert)(void) noexcept;
	STDMETHOD(LockRegion)(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) noexcept;
	STDMETHOD(UnlockRegion)(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) noexcept;
	STDMETHOD(Stat)(STATSTG *pstatstg, DWORD grfStatFlag) noexcept;
	STDMETHOD(Clone)(IStream **ppstm) noexcept;

	void Clear() noexcept;
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