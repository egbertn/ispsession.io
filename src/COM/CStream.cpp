//copyright ADC Cure 2014
#include "StdAfx.h"
#include "CStream.h"

HRESULT CStream::FinalConstruct() throw()
{

	//logModule.Write(L"CStream::ctor\r\n");	
	m_ulSize.QuadPart =
		m_iWritePos.QuadPart =
		m_ulLength.QuadPart = 0;
	//m_refCount = 0;
	m_DidAlloc = false;
	m_WasAssignedArray = false;
	return S_OK;


}
void CStream::FinalRelease() throw()
{
	//	logModule.Write(L"CStream::FinalRelease\r\n");
	Clear();
}

STDMETHODIMP CStream::Read(void* pv, ULONG cb, ULONG *pcbRead) throw()
{
	// Check parameters.
	if (pcbRead != nullptr)
	{
		*pcbRead = 0;
	}
	if (pv == nullptr)
	{
		return STG_E_INVALIDPOINTER;
	}
	if (cb == 0)
	{
		return S_OK;
	}

	// Calculate bytes left and bytes to read.
	ULONG cBytesLeft = m_ulSize.LowPart - m_iWritePos.LowPart;
	ULONG cBytesRead = cb > cBytesLeft ? cBytesLeft : cb;
	PBYTE m_pBuffer = m_alloc.m_pData;
	// If no more bytes to retrieve return S_FALSE.
	if (cBytesLeft == 0)
	{
		return S_FALSE;
	}

	// Copy to users buffer the number of bytes requested or remaining
	memcpy(pv, m_pBuffer + m_iWritePos.QuadPart, cBytesRead);
	m_iWritePos.QuadPart += cBytesRead;

	// Return bytes read to caller.
	if (pcbRead != nullptr)
	{
		*pcbRead = cBytesRead;
	}

	return cb != cBytesRead ? S_FALSE : S_OK;
}

STDMETHODIMP CStream::Write(const void* pv, ULONG cb, ULONG *pcbWritten) throw()
{
	// Check parameters.
	if (pv == nullptr)
	{
		return STG_E_INVALIDPOINTER;
	}
	if (pcbWritten != nullptr)
	{
		*pcbWritten = 0;
	}
	if (cb == 0) return S_OK;

	if (m_iWritePos.QuadPart + cb > m_ulSize.QuadPart)
	{

		m_ulSize.QuadPart += ((m_iWritePos.QuadPart + cb) - m_ulSize.QuadPart);
	}

	// Enlarge the current buffer.
	// eg: 9 + 4bytes > 10 (=13) m_ulLength
	if (m_iWritePos.QuadPart + cb > m_ulLength.QuadPart)
	{
		m_ulLength.QuadPart += ((m_iWritePos.QuadPart + cb) - m_ulLength.QuadPart);
		m_ulLength.QuadPart = MEMALIGN_32(m_ulLength.QuadPart);
		// Grow internal buffer to new size.
#ifdef logModule
		logModule.Write(L"Stream Resize Write %d", m_ulLength);
#endif
		bool result = m_alloc.ReallocateBytes((size_t)m_ulLength.QuadPart);
		// Check for out of memory situation.
		if (result == false)
		{
			Clear();
			return E_OUTOFMEMORY;
		}
	}
	PBYTE m_pBuffer = m_alloc.m_pData;
	// Copy callers memory to internal bufffer and update write position.
	memcpy(m_pBuffer + m_iWritePos.QuadPart, pv, cb);
	m_iWritePos.QuadPart += cb;

	// Return bytes written to caller.
	if (pcbWritten != nullptr)
	{
		*pcbWritten = cb;
	}

	return S_OK;
}


STDMETHODIMP CStream::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition) throw()
{
	//NOTE: we don't bother to write 2G limit memory for our purposes! 
	// Therefore, the HighPart of dbLibMove is ignored
	switch (dwOrigin)
	{
	case STREAM_SEEK_SET:
		m_iWritePos.QuadPart = dlibMove.QuadPart;
		break;
	case STREAM_SEEK_CUR:		
		m_iWritePos.QuadPart += dlibMove.QuadPart;
		break;
	case STREAM_SEEK_END:
		m_iWritePos = m_ulSize;
		break;
	default:
		return STG_E_INVALIDFUNCTION;
		break;
	}
	if (dwOrigin != STREAM_SEEK_END)
	{	
		if (m_iWritePos.QuadPart > m_ulLength.QuadPart)
		{
			m_ulLength = m_ulSize;
			m_ulLength.QuadPart = MEMALIGN_32(m_iWritePos.QuadPart);
			bool result = m_alloc.ReallocateBytes((size_t)m_ulLength.QuadPart);

			if (result == false)
			{
				return E_OUTOFMEMORY;
			}

		}		
	}
	if (plibNewPosition != nullptr)
	{
		plibNewPosition->QuadPart = m_iWritePos.QuadPart;
	}
	return S_OK;

}
STDMETHODIMP CStream::SetSize(ULARGE_INTEGER libNewSize)  throw()
{

	ULARGE_INTEGER li;
	li = libNewSize;
	
	m_ulLength.QuadPart = MEMALIGN_32(li.QuadPart);
	bool result = m_alloc.m_pData == nullptr ? 
		m_alloc.AllocateBytes((size_t)m_ulLength.QuadPart) :
		m_alloc.ReallocateBytes((size_t)m_ulLength.QuadPart);
	
	if (result == false)
	{
		Clear();
		return E_OUTOFMEMORY;
	}	
	m_ulSize = li;
	if (m_iWritePos.QuadPart > li.QuadPart)
	{
		m_iWritePos.QuadPart = li.QuadPart;
	}

	return S_OK;
}
STDMETHODIMP CStream::CopyTo(IStream *, ULARGE_INTEGER, ULARGE_INTEGER *, ULARGE_INTEGER *) throw()
{
	return E_NOTIMPL;
}
STDMETHODIMP CStream::Commit(DWORD)  throw()
{
	return E_NOTIMPL;
}
STDMETHODIMP CStream::Revert() throw()
{
	return E_NOTIMPL;
}
STDMETHODIMP CStream::LockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD) throw()
{
	return E_NOTIMPL;	
}
STDMETHODIMP CStream::UnlockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD)  throw()
{
	return E_NOTIMPL;
}
STDMETHODIMP CStream::Stat(STATSTG *pStatstg, DWORD) throw()
{
	if (pStatstg == nullptr)
	{
		return E_INVALIDARG;
	}
	pStatstg->cbSize = m_ulSize;

	return S_OK;
}
STDMETHODIMP CStream::Clone(IStream **) throw()
{
	return E_NOTIMPL;
}

//STDMETHODIMP CStream::get_Pointer(PBYTE *pointer) throw()
//{
//	//logModule.Write(L"get_Pointer");
//	//TODO:
//	*pointer = m_alloc.m_pData;
//	return *pointer == nullptr ? E_POINTER : S_OK;
//}
//STDMETHODIMP CStream::put_Pointer(PBYTE pointer) throw()
//{
//	//logModule.Write(L"get_Pointer");
//	//TODO:
//	Clear();
//	m_WasAssignedArray = true;
//	//WTF?
//	m_alloc.Attach( pointer);
//	return S_OK;
//}
////
// custom methods
void CStream::Clear()  throw()
{
	if (m_WasAssignedArray == false)
		m_alloc.Free();
	else
		m_alloc.Detach();
	
#ifdef logModule
	logModule.Write(L"ClearStream");
#endif
	m_ulLength.QuadPart =
	m_iWritePos.QuadPart = 0;
}
