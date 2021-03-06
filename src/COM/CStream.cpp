//copyright ADC Cure 2014-2018
#include "StdAfx.h"
#include "CStream.h"

HRESULT CStream::FinalConstruct()  noexcept
{

	//logModule.Write(L"CStream::ctor\r\n");	
	m_ulSize.QuadPart =
		m_iWritePos.QuadPart =
		m_ulLength.QuadPart = 0;
	return S_OK;


}
void CStream::FinalRelease()  noexcept
{
	//	logModule.Write(L"CStream::FinalRelease\r\n");
	Clear();
}

STDMETHODIMP CStream::Read(void* pv, ULONG cb, ULONG *pcbRead)  noexcept
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
	const auto cBytesLeft = m_ulSize.LowPart - m_iWritePos.LowPart;
	ULONG cBytesRead = cb > cBytesLeft ? cBytesLeft : cb;
	
	// If no more bytes to retrieve return S_FALSE.
	if (cBytesLeft == 0)
	{
		return S_FALSE;
	}
	// Copy to users buffer the number of bytes requested or remaining
	memcpy(pv, &m_buf[m_iWritePos.LowPart], cBytesRead);
	m_iWritePos.LowPart += cBytesRead;

	// Return bytes read to caller.
	if (pcbRead != nullptr)
	{
		*pcbRead = cBytesRead;
	}

	return cb != cBytesRead ? S_FALSE : S_OK;
}

STDMETHODIMP CStream::Write(const void* pv, ULONG cb, ULONG *pcbWritten)  noexcept
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
		m_buf.resize(static_cast<size_t>(m_ulLength.QuadPart));
	}
	// Copy callers memory to internal bufffer and update write position.
	memcpy(&m_buf[ m_iWritePos.LowPart], pv, cb);
	m_iWritePos.QuadPart += cb;

	// Return bytes written to caller.
	if (pcbWritten != nullptr)
	{
		*pcbWritten = cb;
	}

	return S_OK;
}


STDMETHODIMP CStream::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)  noexcept
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
		
	}
	if (dwOrigin != STREAM_SEEK_END)
	{	
		if (m_iWritePos.QuadPart > m_ulLength.QuadPart)
		{
			m_ulLength = m_ulSize;
			m_ulLength.QuadPart = MEMALIGN_32(m_iWritePos.QuadPart);
			m_buf.resize(static_cast<size_t>(m_ulLength.QuadPart));
		}		
	}
	if (plibNewPosition != nullptr)
	{
		plibNewPosition->QuadPart = m_iWritePos.QuadPart;
	}
	return S_OK;

}
STDMETHODIMP CStream::SetSize(ULARGE_INTEGER libNewSize)   noexcept
{

	ULARGE_INTEGER li;
	li = libNewSize;
	m_ulLength.QuadPart = MEMALIGN_32(li.QuadPart);
	m_buf.resize(static_cast<size_t>(m_ulLength.QuadPart));
	m_ulSize = li;
	if (m_iWritePos.QuadPart > li.QuadPart)
	{
		m_iWritePos.QuadPart = li.QuadPart;
	}

	return S_OK;
}
STDMETHODIMP CStream::CopyTo(
	IStream*      pstm,         
	ULARGE_INTEGER  cb,           
	ULARGE_INTEGER* pcbRead,      
	ULARGE_INTEGER* pcbWritten)  noexcept
{
	HRESULT        hr = S_OK;
	BYTE           tmpBuffer[512];
	ULONG          bytesRead, bytesWritten, copySize;
	ULARGE_INTEGER totalBytesRead;
	ULARGE_INTEGER totalBytesWritten;


	AtlTrace(L"(%p, %p, %d, %p, %p)\n", this, pstm,
		cb.u.LowPart, pcbRead, pcbWritten);

	if (pstm == nullptr)
		return STG_E_INVALIDPOINTER;

	totalBytesRead.QuadPart = 0;
	totalBytesWritten.QuadPart = 0;
	const ULARGE_INTEGER backupWRitePos = m_iWritePos;
	m_iWritePos.QuadPart = 0;

	while (cb.QuadPart > 0 && hr == S_OK)
	{
		
		copySize = min(cb.u.LowPart, sizeof(tmpBuffer));
		hr = Read(tmpBuffer, copySize, &bytesRead);
		if (FAILED(hr))
			break;

		totalBytesRead.QuadPart += bytesRead;

		if (bytesRead)
		{
			hr = pstm->Write(tmpBuffer, bytesRead, &bytesWritten);
			if (FAILED(hr))
				break;

			totalBytesWritten.QuadPart += bytesWritten;
		}

		if (bytesRead != copySize)
			cb.QuadPart = 0;
		else
			cb.QuadPart -= bytesRead;
	}
	m_iWritePos = backupWRitePos;

	if (pcbRead) pcbRead->QuadPart = totalBytesRead.QuadPart;
	if (pcbWritten) pcbWritten->QuadPart = totalBytesWritten.QuadPart;

	return hr;

}
STDMETHODIMP CStream::Commit(DWORD)  noexcept
{
	return E_NOTIMPL;
}
STDMETHODIMP CStream::Revert() noexcept
{
	return E_NOTIMPL;
}
STDMETHODIMP CStream::LockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD) noexcept
{
	return E_NOTIMPL;	
}
STDMETHODIMP CStream::UnlockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD)  noexcept
{
	return E_NOTIMPL;
}
STDMETHODIMP CStream::Stat(STATSTG *pStatstg, DWORD) noexcept
{
	if (pStatstg == nullptr)
	{
		return E_INVALIDARG;
	}
	pStatstg->cbSize = m_ulSize;

	return S_OK;
}

STDMETHODIMP CStream::Clone(IStream **) noexcept
{
	return E_NOTIMPL;
}

//STDMETHODIMP CStream::get_Pointer(PBYTE *pointer) noexcept
//{
//	//logModule.Write(L"get_Pointer");
//	//TODO:
//	*pointer = m_alloc.m_pData;
//	return *pointer == nullptr ? E_POINTER : S_OK;
//}
//STDMETHODIMP CStream::put_Pointer(PBYTE pointer) noexcept
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
void CStream::Clear()  noexcept
{
	
		//m_alloc.Free();
		m_buf.clear();
	/*else
		m_alloc.Detach();*/
	
#ifdef logModule
	logModule.Write(L"ClearStream");
#endif
	m_ulLength.QuadPart =
	m_iWritePos.QuadPart = 0;
}
