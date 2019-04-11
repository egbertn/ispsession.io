#pragma once
#include <string>
#ifndef CComBSTR
#define CComBSTR CComBSTR2
#endif
#define CComVariant CComVariant2
namespace ATL
{

//typedef  HRESULT  (__stdcall*HASHDATA2) (LPBYTE, DWORD, LPBYTE, DWORD);

/////////////////////////////////////////////////////////////////////////////
// CComBSTR2
class CComBSTR2
{
public:
	BSTR m_str;
	CComBSTR2() throw()
	{
		m_str = NULL;
	}
#ifdef _ATL_CCOMBSTR_EXPLICIT_CONSTRUCTORS
	explicit CComBSTR(_In_ int nSize)
#else
	CComBSTR(_In_ int nSize)
#endif
	{
		//if (nSize == 0) //BUG it should be possible to assign a L"" string 
		if (nSize < 0)
		{
			AtlThrow(E_INVALIDARG);
		}
		m_str = NULL;

		HRESULT hr = SetLength(nSize);
		if (FAILED(hr))
			AtlThrow(hr);
		ZeroMemory(m_str, nSize * sizeof(wchar_t));
		
	}
	CComBSTR2(_In_ int nSize, _In_reads_opt_(nSize) LPCOLESTR sz)
	{
		if (nSize < 0)
		{
			AtlThrow(E_INVALIDARG);
		}
		
		if (nSize == 0)
			m_str = NULL;
		else
		{
			m_str = ::SysAllocStringLen(sz, nSize);
			if (!*this)
			{
				AtlThrow(E_OUTOFMEMORY);
			}
		}
	}
	CComBSTR2(_In_opt_z_ LPCOLESTR pSrc)
	{
		if (pSrc == NULL)
			m_str = NULL;
		else
		{
			m_str = ::SysAllocString(pSrc);
			if (!*this)
			{
				AtlThrow(E_OUTOFMEMORY);
			}
		}
	}
	CComBSTR2(_In_ const CComBSTR& src)
	{
		m_str = src.Copy();
		if (!!src && !*this)
        {
			AtlThrow(E_OUTOFMEMORY);
        }
	}
	CComBSTR2(_In_ REFGUID guid)
	{
		OLECHAR szGUID[64];
		ATLPREFAST_SUPPRESS(6031)
			::StringFromGUID2(guid, szGUID, 64);
		ATLPREFAST_UNSUPPRESS()
			m_str = ::SysAllocString(szGUID);
		if (!*this)
		{
			AtlThrow(E_OUTOFMEMORY);
		}
	}

	CComBSTR2& operator=(_In_ const CComBSTR& src)
	{
		if (m_str != src.m_str)
		{
			if (::SysReAllocStringLen(&m_str, src, src.Length()) == FALSE)			
				AtlThrow(E_OUTOFMEMORY);
		}
		return *this;
	}

	CComBSTR2& operator=(_In_opt_z_ LPCOLESTR pSrc)
	{
		if (pSrc != m_str)
		{
			if (pSrc != NULL)
			{
				if (::SysReAllocString(&m_str, pSrc) == FALSE)				
					AtlThrow(E_OUTOFMEMORY);
			}
			else		
			{
				Empty();
			}
		}
		return *this;
	}
	CComBSTR2(_In_ const CComBSTR2&& src)
	{
		m_str = src.Copy();
		if (!!src && !*this)
		{
			AtlThrow(E_OUTOFMEMORY);
		}
	}

	CComBSTR2& operator=(decltype(__nullptr)) throw()
	{
		::SysFreeString(m_str);
		m_str = NULL;
		return *this;
	}
	CComBSTR2& operator=(_Inout_ CComBSTR2&& src)
	{
		if (m_str != src.m_str)
		{
			//smell: should be ByteLen eg. binary copy
			if (::SysReAllocStringLen(&m_str, src.m_str, src.Length()) == FALSE)			
				AtlThrow(E_OUTOFMEMORY);			
		}
		return *this;
	}

	inline CComBSTR2::~CComBSTR2() throw()
	{
		::SysFreeString(m_str);
	}
	
	static ULONG GetStreamSize(BSTR bstr)
	{
		ULONG ulSize = sizeof(ULONG);
		if (bstr != NULL)
		{
			ulSize += SysStringByteLen(bstr) + sizeof(OLECHAR);
		}

		return ulSize;
	}
	
	unsigned int __stdcall Length() const throw()
	{
		return ::SysStringLen(m_str);
	}
	unsigned int __stdcall ByteLength() const throw()
	{
		return ::SysStringByteLen(m_str);
	}
	operator BSTR() const throw()
	{
		return m_str;
	}
#ifndef ATL_CCOMBSTR_ADDRESS_OF_ASSERT
// Temp disable CComBSTR::operator& Assert
#define ATL_NO_CCOMBSTR_ADDRESS_OF_ASSERT
#endif

	BSTR* operator&() throw()
	{
#ifndef ATL_NO_CCOMBSTR_ADDRESS_OF_ASSERT
		ATLASSERT(!*this);
#endif
		return &m_str;
	}
	// returns a UTF-8 encoded string of type std::string
	std::string __stdcall ToString() const
	{
		// null is handled correctly
		CComBSTR utf8;
		auto copyByteSTring = ToByteString();
		utf8.Attach(copyByteSTring);
		return std::move( std::string((PSTR)utf8.m_str, utf8.ByteLength()));
	}
	std::wstring __stdcall ToWString() const throw()
	{
		// null is handled correctly
		return std::move(std::wstring(m_str, Length()));
	}
	BSTR __stdcall Copy() const throw()
	{
		if (*this == NULL)
			return NULL;

		unsigned int copyLen = ByteLength();
		BSTR retVal = ::SysAllocStringByteLen((char*)m_str, copyLen);			
		return retVal;
		
	
	}
	_Check_return_ HRESULT CopyTo(_Outptr_result_maybenull_ _Result_nullonfailure_ BSTR* pbstr) const throw()
	{
		ATLASSERT(pbstr != NULL);
		if (pbstr == NULL)
		{
			return E_POINTER;
		}
		*pbstr = Copy();

		if ((*pbstr == NULL) && (m_str != NULL))
		{
			return E_OUTOFMEMORY;
		}
		return S_OK;
	}

	// *** returns true if length equals zero characters or when unallocated(null pointer)
	bool __stdcall IsEmpty (void) const throw()
	{
		return m_str == NULL || ByteLength() == 0;
	}
	/* added by may 2005 e.n. needs #include 'wchar.h'*/
	STDMETHODIMP Format(_In_ PCWSTR pszFormat, _In_ va_list args) throw()
	{
		size_t len = _vscwprintf( pszFormat, args );
		HRESULT hr = S_OK;		
		hr = SetLength((UINT)len) ;

		if(SUCCEEDED(hr) && len > 0)
			if (vswprintf( m_str, len+1, pszFormat, args ) < 0)
				hr = E_INVALIDARG;
		
		return hr;
	}

	/* added by may 2005 e.n. needs #include 'wchar.h'*/
	HRESULT __cdecl Format(_In_ PCWSTR pszFormat, ...) throw()
	{
		va_list args;			
		va_start( args, pszFormat );		
		HRESULT hr = Format(pszFormat, args);
		va_end(args);
		
		return hr;
	}
private:
	STDMETHODIMP Insert(_In_ unsigned int atPosition, _In_opt_ LPCOLESTR lpsz, _In_ unsigned int nLen) throw()
	{
		unsigned int curLen = Length();
		HRESULT hr = S_OK;
		if (atPosition > curLen || lpsz == NULL) 
			hr = E_INVALIDARG;
		else
			hr = SetLength(curLen + nLen);
		if (SUCCEEDED(hr) && curLen != 0 && nLen != 0)
		{
			
			wmemmove_s(&m_str[atPosition + nLen], (curLen + nLen) - atPosition, &m_str[atPosition], 
				curLen - atPosition);			
			Checked::wmemcpy_s(&m_str[atPosition], (curLen + nLen) - atPosition, lpsz, nLen );
		}
		return hr;
	}
public:
	STDMETHODIMP  Insert(_In_ unsigned int atPosition, _In_opt_ PCWSTR value) throw()
	{
		return Insert(atPosition, value, (unsigned int)wcslen(value));
	}
	STDMETHODIMP Insert(_In_ unsigned int atPosition, _In_ const CComBSTR& value) throw()
	{
		return Insert(atPosition, value.m_str, value.Length());
	}

	STDMETHODIMP TrimStart(_In_opt_ PCWSTR trimwhat = NULL) throw()
	{
		PCWSTR trim = trimwhat == NULL? L" ": trimwhat;
		if (IsEmpty()) return S_OK;
		unsigned int trimLen = (unsigned int)wcslen(trim);
		while(StartsWith(trim))
			Remove(0, trimLen);
		return S_OK;
	}
	STDMETHODIMP TrimEnd(_In_opt_ PCWSTR trimwhat = NULL) throw()
	{
		PCWSTR trim = trimwhat == NULL? L" ": trimwhat;
		if (IsEmpty()) return S_OK;
		unsigned int trimLen = (unsigned int)wcslen(trim);
		while(EndsWith(trim))
			SetLength(Length() - trimLen);
		return S_OK;
	}

	//** removes in-place characters from this BSTR
	STDMETHODIMP Remove(
		//** zero based starting position where you start to remove characters
		//** if this number is outside valid bounds, E_INVALIDARG is returned
		_In_ unsigned int startIndex, 
		//** the number of characters, you want to remove
		//** if this number is outside valid bounds, it is corrected
		_In_ unsigned int count) throw()
	{
		unsigned int maxIdx = Length();
		// avoid buffer overflow
		if (count + startIndex > maxIdx) count = maxIdx - startIndex;
		HRESULT hr = S_OK;
		if (startIndex < maxIdx) 				
		{
			if (maxIdx - startIndex - count != 0)			
				//copy back, overlapped memory
				wmemmove_s(&m_str[startIndex], maxIdx - startIndex, &m_str[startIndex + count] , 
					maxIdx - startIndex - count);	
			// shrink to new length
			
			hr = SetLength(maxIdx - count);
		}
		else 
			hr = E_INVALIDARG;
		
		return hr;
	}
	/// <summary>original string john smith
	///  merge with west at position 6
	/// result john westh
	/// won't extend string length!
	/// </summary>
	void __stdcall MergeString(_In_ unsigned int startIndex, _In_ const BSTR value) throw()
	{

		unsigned int maxIdx = Length();
		if (startIndex > maxIdx || value == NULL) return; // illegal operation
		unsigned int mergeLen = SysStringLen(value);
		if (mergeLen + startIndex > maxIdx)
			mergeLen = maxIdx - startIndex;
		Checked::wmemcpy_s(&m_str[startIndex], maxIdx - startIndex, value, mergeLen);
	}
	BSTR __stdcall Substring(_In_ unsigned int startIndex)  const throw()
	{
		unsigned int maxIdx = Length();
		if (m_str != NULL && startIndex >= 0 && startIndex <= maxIdx)
		{
			return ::SysAllocStringLen(m_str + startIndex, maxIdx - startIndex);
		}
		else
			return NULL;
	}
	
	STDMETHODIMP SetByteLength(_In_ unsigned int length) throw()
	{
		return _SetByteLength(&m_str, length);
	}
	
	// Cuts the length to specified but does not clear contents
	STDMETHODIMP SetLength(_In_ unsigned int length) throw()
	{
		return _SetLength(&m_str, length);
	}
private:
	static STDMETHODIMP _SetByteLength(_Inout_ BSTR *str, _In_ unsigned int length) throw()
	{
		BSTR Copy = NULL;
		if (*str != NULL)
		{
			if ((length % 2) == 0)
			{
				SysReAllocStringLen(str, NULL, length / 2);
			}
			else
			{
				Copy = SysAllocStringByteLen(NULL, length);
				memset(Copy, 0, length);
				auto oldLen = ::SysStringByteLen(*str);
				memcpy(Copy, *str, min(oldLen, length));
				SysFreeString(*str);
				*str = Copy;
			}
		}
		else
		{
			*str = SysAllocStringByteLen(NULL, length);
		}
		return *str == NULL ? E_OUTOFMEMORY : S_OK;
	}
	static STDMETHODIMP  _SetLength(_Inout_ BSTR * str, _In_ unsigned int length) throw()
	{
		return ::SysReAllocStringLen(str, NULL, length) == FALSE ? E_OUTOFMEMORY : S_OK;
		
	}
public:
	int __stdcall TokenCount(_In_ PCWSTR find, _In_ bool caseInsensitive) throw()
	{
		int strLen = (int)Length();
		int tokenCount = 0;
		int findLen = (int)wcslen(find);
		if (findLen < strLen) 
		{
			for(int x = 0; 
				(x = IndexOf((const PWSTR)find, x, caseInsensitive)) >= 0;)
			{
				x += findLen;
				tokenCount++;
			}
		}
		return tokenCount;
	}
	/// <summary>
	/// Replaces a find token with a specified token
	/// By E.N.
	/// </summary>	
	/// <param name="find">token to be replace</param>
	/// <param name="replace">token that will replace</param>
	/// <param name="caseInsensitive">if true, will do a case insensitive replacement</param>
	/// <example>
	/// CComBSTR2 myReplace(L"the dog jumps over the");
	/// myReplace(CComBSTR2(L"the"), CComBSTR2(L"big"));
	/// </example>
	/// <returns>The example would modify the string to "big dog jumps over big" </returns>	
	STDMETHODIMP Replace(_In_opt_ BSTR find, _In_opt_ BSTR replace, _In_ bool caseInsensitive) throw()
	{
		HRESULT hr = S_OK;
		if (m_str == NULL || find == NULL || replace == NULL)
			hr = E_INVALIDARG;
		else
		{
			int countTokens = TokenCount(find, caseInsensitive);
			if (countTokens == 0) return hr;
			int findLen = SysStringLen(find);
			int replaceLen = SysStringLen(replace);
			int lengthDiff = replaceLen - findLen;
			int oldLen = (int)Length() ;
			int newLen = oldLen + (countTokens * lengthDiff);
			if (lengthDiff > 0)
			{
				hr = SetLength(newLen);
				if (FAILED(hr)) return hr;
			}
			
			for(int x = 0; 
				(x = IndexOf(find, x, caseInsensitive)) >= 0				
				; )
			{
				int offset = x + findLen;
				if (lengthDiff != 0)
				{
					wmemmove_s(&m_str[offset + lengthDiff], 
						(lengthDiff < 0 ? oldLen : newLen) - offset, 
						&m_str[offset], 
						oldLen - offset);
					oldLen += lengthDiff;
				}										
				if (replaceLen > 0) 
				{
					MergeString(x, replace);
					x += replaceLen;			
				}
			}
			if (lengthDiff < 0)
			{
				hr = SetLength(newLen);				
			}
		}
		return hr;
	}

	/// <summary>
	/// Replaces a find token with a specified token
	/// <param name="find">token to be replace</param>
	/// <param name="replace">token that will replace</param>
	/// </summary>	
	/// <example>
	/// CComBSTR2 myReplace(L"the dog jumps over the");
	/// myReplace(CComBSTR2(L"the"), CComBSTR2(L"big"));
	/// </example>
	/// <returns>The example would modify the string to "big dog jumps over big" </returns>	
	STDMETHODIMP Replace(_In_opt_ const BSTR find, _In_opt_ const BSTR replace) throw()
	{
		return Replace(find, replace, false);
	}

	SAFEARRAY* __stdcall Split(_In_opt_ PCWSTR expression, _In_ const bool caseInsenstive)  const throw()
	{
		return Split(CComBSTR(expression), caseInsenstive);
	}
	/// <summary>
	/// Split and copies this instance of CComBSTR2 into a SAFEARRAY* of VTYPE = VT_BSTR
	/// </summary>	
	/// <example>
	/// CComSafeArray<BSTR> myArray;
	/// CComBSTR2 joined(L"John|Smith");
	/// myArray.Attach(joined.Split(CComBSTR2(L"|")))
	/// </example>
	/// <returns>The example would return a safearray with 2 VT_BSTR elements containing "John" and "Smith"</returns>	
	SAFEARRAY* __stdcall Split(_In_ const CComBSTR& expression, _In_ const bool caseInsensitive)  const
	{
		SAFEARRAY* retval = NULL;
		HRESULT hr = S_OK;
		if (m_str == NULL)
			AtlThrow(E_POINTER);
		else
		{
			unsigned int exprLen = expression.Length();
			unsigned int mLen = Length();
			
			int x = 0;
			//contains the number of found expression tokens
			unsigned int found = mLen == 0 ? -1 : 0;					
			//find until no more...
			if (expression != NULL && found >= 0)
			{
				for (;;)
				{
					x = IndexOf(expression, x, caseInsensitive);
					if (x == -1) break;				
					found++;
					x += exprLen;				
				}				
			}			
			retval = ::SafeArrayCreateVector(VT_BSTR, 0, found + 1); 
			if (retval == NULL)
				return retval;
			
			else if (mLen > 0)
			{				
				BSTR* paBSTR  ;//(BSTR*)retval->pvData;
				hr = SafeArrayAccessData(retval, (void**)&paBSTR);
				if (hr == S_OK)
				{	int prevPos = 0;
					x = 0;
					for (unsigned int curEl = 0; curEl <= found; curEl++)
					{
						x = IndexOf(expression, x, caseInsensitive);					
						paBSTR[curEl] = x < 0 ? Substring(prevPos) : Substring(prevPos, x - prevPos);
						if (paBSTR[curEl] == NULL)
						{
							hr = E_OUTOFMEMORY;
							break;
						}
						x += exprLen;	
						prevPos = x;					
					}				
					SafeArrayUnaccessData(retval);					
				}
			}
			if (FAILED(hr))
			{
				AtlTrace(L"general split fail %x", hr);
				if (retval != NULL) 
					::SafeArrayDestroy(retval);
				retval = NULL;
				AtlThrow(hr);
			}				
		}
		return retval;
	}
	SAFEARRAY* __stdcall Split(_In_opt_ PCWSTR expression)  const throw()
	{
		return Split(CComBSTR(expression), false);
	}

	/// <summary>
	/// Added by E.N.
	/// Joins a SAFEARRAY to a single BSTR
	/// </summary>	
	/// <example>
	/// CComSafeArray&lt;BSTR&gt; myArray;	
	/// myArray.Add(CComBSTR2(L"John"));
	/// myArray.Add(CComBSTR2(L"Smith"));
	/// CComBSTR2 joined;
	/// joined.Attach(CComBSTR2::Join(myArray.m_psa, CComBSTR2(L"|") ) );	/// 
	/// </example>
	/// <returns>The example would return "John|Smith"</returns>	
	static BSTR __stdcall Join(_In_opt_ SAFEARRAY *psa, _In_ const BSTR delimiter) //throw()
	{
		BSTR retval = NULL;
		HRESULT hr = S_OK;
		if (psa != NULL && psa != NULL)
		{
			VARTYPE vt = VT_EMPTY;
			unsigned int delLen = ::SysStringByteLen(delimiter);
			UINT dims = ::SafeArrayGetDim(psa);
			hr = ::SafeArrayGetVartype(psa, &vt);
			if (vt != VT_BSTR || dims != 1) 
				AtlThrow(E_INVALIDARG);
			
			LONG ubound, elements, lbound;			
			::SafeArrayGetUBound(psa, 1, &ubound);
			::SafeArrayGetLBound(psa, 1, &lbound);
			elements = ubound - lbound + 1;
			
			BSTR* paBSTR;
			hr = ::SafeArrayAccessData(psa,(void**) &paBSTR);			
			if (FAILED(hr))
			{
				AtlThrow(hr);
			}
			unsigned int totalLen = 0;
			ULONG els = elements;
			while(els != 0)
			{
				if (paBSTR[--els] != NULL)
					totalLen += ::SysStringByteLen(paBSTR[els]);
				if (els != 0)
					totalLen += delLen;
			}
			hr = _SetByteLength(&retval, totalLen);
			if (FAILED(hr))
			{
				::SafeArrayUnaccessData(psa);
				AtlThrow(hr);
			}
			else
			{
				totalLen = 0;
				els = elements;
				ULONG curel = 0;
				while (els != 0)
				{
					els--;
					if (paBSTR[curel] != NULL)
					{
						unsigned int curLen = ::SysStringByteLen(paBSTR[curel]);
						Checked::memcpy_s(((PSTR)retval) + totalLen, curLen, paBSTR[curel], curLen);
						totalLen += curLen;
					}
					curel++;
					if (delLen != 0 && curel < elements)
					{
						Checked::memcpy_s(((PSTR)retval) + totalLen, delLen, delimiter, delLen);
						totalLen += delLen;
					}					
				}
			} 
			::SafeArrayUnaccessData(psa);//success allocate string				
			
		}
		return retval;
	}
	/// <summary>
	/// Added by E.N.
	/// Returns a new instance of a BSTR which is a Substring starting at the 'startindex' character
	/// </summary>	
	/// <example>
	/// CComBSTR2 john(L"John Smith"), subbed;
	/// subbed.Attach(john.Substring(5));
	/// </example>
	/// <returns>The example would return "Smith"</returns>	
	BSTR __stdcall Substring(_In_ const unsigned int startIndex, _In_ unsigned int length)  const throw()
	{
		unsigned int maxIdx = Length();
		//if (length < 0) length = 0;
		if (startIndex + length > maxIdx) 
			length = maxIdx - startIndex;
		if (m_str != NULL && startIndex >= 0 && startIndex <= maxIdx)
		{
			return ::SysAllocStringLen(m_str + startIndex, length);
		}
		else
			return NULL;
	}
private:
	bool __stdcall local_Test(_In_opt_ PCWSTR src, unsigned int startIndex, bool caseInsensitive)  const throw()
	{
		bool retval = false;
		if (src != NULL)
		{
			unsigned int compLen = (unsigned int)wcslen(src);
			unsigned int thisLen = Length();
			if (compLen <= thisLen)
			{
				DWORD dwCompFlags = caseInsensitive ? NORM_IGNORECASE : 0;
				retval = ::CompareStringW(::GetThreadLocale(), dwCompFlags,
					&m_str[thisLen - startIndex], compLen, src, compLen) == CSTR_EQUAL;
			}
		}
		return retval;
	}
public:

	bool __stdcall EndsWith(_In_opt_ PCWSTR src, bool caseInsenstive) const throw()
	{
		return local_Test(src, (unsigned int) wcslen(src),  caseInsenstive);
	}
	bool __stdcall EndsWith(_In_opt_ PCWSTR src) const throw()
	{
		return local_Test(src, (unsigned int)wcslen(src), false);
	}
	bool __stdcall StartsWith(_In_opt_ PCWSTR src, bool caseInsensitive) const throw()
	{
		return local_Test(src, Length(), caseInsensitive);
	}
	bool __stdcall StartsWith(_In_opt_ PCWSTR src) const throw()
	{
		return local_Test(src, Length(), false);
	}
	int __stdcall LastIndexOf(_In_ const wchar_t src, _In_opt_ const unsigned int startIndex = 0, _In_ const bool caseInsensitive = false)  const throw()
	{
		wchar_t  src2[] = {src, NULL}; //create zero terminated PWSTR
		return LastIndexOf(src2, startIndex, caseInsensitive);
	}
	int __stdcall LastIndexOf(_In_opt_ const wchar_t *src, _In_ const unsigned int startIndex = 0, _In_ const bool caseInsensitive = false, unsigned int count = 0)  const throw()
	{
		int result = -1;
		if (m_str != NULL && src != NULL)
		{
			LCID lcid = ::GetThreadLocale();
			DWORD dwCmpFlags = caseInsensitive ? NORM_IGNORECASE : 0;
		
			unsigned int compLen = (unsigned int)wcslen(src);
			unsigned int maxLen = Length();
			unsigned int examinedChars = 0;
			if (compLen <= maxLen)
			{
				for(unsigned int x = maxLen - compLen; x >= startIndex; )
				{
					bool doCompare = caseInsensitive ? true : m_str[x] == src[0];

					if (doCompare)
						doCompare=  ::CompareStringW(lcid, dwCmpFlags, &m_str[x], 
								compLen, src, compLen) == CSTR_EQUAL	;
					if (doCompare)
					{
						result = x;
						break;
					}
					if (x-- == 0 || (examinedChars++ == count && count != 0)) break;					
				}
			}
		}
		return result;
	}

	int __stdcall IndexOf(_In_ const wchar_t src, _In_ const unsigned int startIndex = 0, _In_ const bool caseInsensitive = false, unsigned int count = 0) const throw()
	{
		wchar_t  src2[] = {src, NULL}; //create zero terminated PWSTR
		return IndexOf(src2, startIndex, caseInsensitive, count);
	}

	//
	// Addded by E.N.
	//
	int __stdcall IndexOf(_In_opt_ const PWSTR src, _In_ const unsigned int startIndex = 0, _In_ const bool caseInsensitive = false, unsigned int count = 0) const throw()
	{
		int result = -1;
		
		if (m_str != NULL && src != NULL)
		{			
			LCID lcid = ::GetThreadLocale();
			DWORD dwCmpFlags = caseInsensitive ? NORM_IGNORECASE : 0;
			unsigned int maxLen = Length();
			unsigned int compLen = (unsigned int)wcslen(src);
			unsigned int examinedChars = 0;
			if (compLen <= maxLen && compLen > 0)
			{
				for(int x = startIndex; (x + compLen <= maxLen) || (examinedChars++ == count && count != 0); x++)
				{
					bool doCompare = caseInsensitive ? true : 
						m_str[x] == src[0];

					if (doCompare)
						doCompare=  ::CompareStringW(lcid, dwCmpFlags, &m_str[x], 
								compLen, src, compLen) == CSTR_EQUAL	;
					if (doCompare)				
					{
						result = x;
						break;
					}						
				}
			}					
		}
		return result;
	}

	// copy BSTR to VARIANT
	_Check_return_ HRESULT CopyTo(_Out_ VARIANT *pvarDest) const throw()
	{
		ATLASSERT(pvarDest != NULL);
		HRESULT hRes = E_POINTER;
		if (pvarDest != NULL)
		{
			pvarDest->vt = VT_BSTR;
			pvarDest->bstrVal = Copy();

			if (pvarDest->bstrVal == NULL && m_str != NULL)
            {
				hRes = E_OUTOFMEMORY;
            }
			else
            {
				hRes = S_OK;
            }
		}
		return hRes;
	}
	void __stdcall Attach(_In_opt_ BSTR src) throw()
	{
		if (m_str != src)
		{
			SysFreeString(m_str);
			m_str = src;
		}
	}

	BSTR Detach() throw()
	{
		BSTR s = m_str;
		m_str = NULL;
		return s;
	}

	void Empty() throw()
	{
		::SysFreeString(m_str);
		m_str = NULL;
	}

	bool operator!() const throw()
	{
		return (m_str == NULL);
	}
	
	_Check_return_ STDMETHODIMP Append(_In_ const CComBSTR2& bstrSrc) throw()
	{
		return AppendBSTR(bstrSrc.m_str);
	}
	/*HRESULT Append(const CComVariant2& pVarSrc) throw()
	{
		return Append(pVarSrc.pvarVal);
	}*/
	//Added by E.N.
	_Check_return_ STDMETHODIMP Append(_In_ const VARIANT& pVar) throw()
	{
		HRESULT hr = S_OK;
		if (pVar.vt != VT_BSTR)
		{
			VARIANT v = {0};
			hr = ::VariantChangeTypeEx(&v,(VARIANT*)&pVar, 
				::GetThreadLocale(), 0, VT_BSTR);
			if (hr == S_OK) hr = AppendBSTR(v.bstrVal);
			VariantClear(&v);
		}
		else
			hr = AppendBSTR(pVar.bstrVal);

		return hr;
	}

	_Check_return_ STDMETHODIMP Append(_In_opt_  LPCOLESTR lpsz) throw()
	{
		__analysis_assume(lpsz);
		return Append(lpsz, (UINT)ocslen(lpsz));
	}
	// a BSTR is just a LPCOLESTR so we need a special version to signify
	// that we are appending a BSTR
	_Check_return_ STDMETHODIMP AppendBSTR(_In_opt_ BSTR p) throw()
	{
		return Append((LPCOLESTR)p, ::SysStringLen(p));
	}
	_Check_return_ STDMETHODIMP Append(_In_opt_count_(nLen) LPCOLESTR lpsz, int nLen) throw()
	{
		if (lpsz == NULL || (m_str != NULL && nLen == 0))
			return S_OK;
		int n1 = Length();
		HRESULT hr = SetLength(n1 + nLen);
		if ( FAILED(hr))		
			return hr;	
		Checked::wmemcpy_s(m_str+n1, (n1 + nLen), lpsz, nLen);
		return S_OK;
	}
	_Check_return_ STDMETHODIMP Append(_In_ char ch) throw()
	{
		OLECHAR chO = ch;

		return( Append( &chO, 1 ) );
	}

	_Check_return_ STDMETHODIMP Append(_In_ wchar_t ch) throw()
	{
		return( Append( &ch, 1 ) );
	}

	_Check_return_ STDMETHODIMP AppendBytes(_In_opt_count_(nLen) const char* lpsz, int nLen) throw()
	{
		if (lpsz == NULL || nLen == 0)
			return S_OK;
		int n1 = ByteLength();
		BSTR b;
		b = ::SysAllocStringByteLen(NULL, n1+nLen);
		if (b == NULL)
			return E_OUTOFMEMORY;
		memcpy_s(b, n1+nLen, m_str, n1);
		memcpy_s(((char*)b)+n1, nLen, lpsz, nLen);
		*((OLECHAR*)(((char*)b)+n1+nLen)) = NULL;
		Empty();
		m_str = b;
		return S_OK;
	}
	static int __stdcall CountChar(_In_ const BSTR a, _In_ wchar_t theChar) throw()
	{
		int retval = 0;
		if (a != NULL) 
		{
			unsigned int x  = ::SysStringLen(a);					
			while (x != 0)			
				if (a[--x] == theChar) retval++;		
		}
		return retval;
	}
	//returns -1 for lesser (<); 0 for equality and 1 for GT (>)
	static int __stdcall Compare(_In_ const BSTR a, _In_ const BSTR b, _In_ bool ignoreCase, _In_ bool ignoreDiacritics, bool ignoreSymbols) throw()
	{
		int retval = 0;
		DWORD compareFlags = ignoreCase ? NORM_IGNORECASE : 0;
		if (ignoreDiacritics) compareFlags |= NORM_IGNORENONSPACE;
		if (ignoreSymbols) compareFlags |= NORM_IGNORESYMBOLS;

		if (a == NULL && b == NULL)
			retval = 0;
		else if (a == NULL)
			retval = -1;
		else if (b == NULL)
			retval = 1;
		else
		{
			retval = ::VarBstrCmp(a, b, ::GetThreadLocale(), compareFlags) - 1;
		}
		return retval;
	}
	
	int __stdcall CompareTo(_In_ const BSTR otherBstr) throw()
	{
		return Compare(m_str, otherBstr, false, false, false);
	}

	int __stdcall CompareTo(_In_ const BSTR otherBstr, _In_ bool ignoreCase) throw()
	{
		return Compare(m_str, otherBstr, ignoreCase, false, false);
	}

	int __stdcall CompareTo(_In_ const BSTR otherBstr, _In_ bool ignoreCase, _In_ bool ignoreDiacritics) throw()
	{
		return Compare(m_str, otherBstr, ignoreCase, ignoreDiacritics, false);
	}

	int __stdcall CompareTo(_In_ const BSTR otherBstr, _In_ bool ignoreCase, _In_ bool ignoreDiacritics, bool ignoreSymbols) throw()
	{
		return Compare(m_str, otherBstr, ignoreCase, ignoreDiacritics, ignoreSymbols);
	}
	
	unsigned long GetHashCode() throw() 
	{
		//if insufficient, shlwapi::HashData could be used 
		unsigned long hash=0;
		//HASHDATA2 hashdata2 = NULL;
		//hash = LHashValOfName(::GetThreadLocale(), m_str);
		
		//TCHAR shlwapi[] = {'S', 'H', 'L', 'W', 'A', 'P', 'I', '.', 'D', 'L', 'L', '\0'};
		//HINSTANCE hinst = LoadLibrary(shlwapi);
		//char procName[]={'H', 'a', 's', 'h', 'D', 'a', 't', 'a', '\0'}; // procName		
		//hashdata2 = (HASHDATA2) GetProcAddress(hinst, procName); //
//		HRESULT hr = S_OK;
		if (!IsEmpty())
		{
			//compress unicode before hashing
			CComBSTR lpza;
			lpza.Attach(ToByteString());
			//CW2AEX<> lpza(m_str, CP_UTF8);
			
			//if (hashdata2 != NULL)
			HRESULT	hr = HashData((PBYTE)lpza.m_str, lpza.ByteLength(),(LPBYTE) &hash, sizeof(unsigned long));
			if (hr != S_OK) hash = 0xFFFFFFFF;
		}
		//FreeLibrary(hinst);
		return hash;
	}

	// added by e.n.
	static void __stdcall StringReverse(_In_opt_ const  BSTR s) throw()
	{
		//CComBSTR2 temp;
		//HRESULT  hr = temp.AssignBSTR(s); //create a copy
		UINT slen = SysStringLen(s), y = 0;

		while(y < slen--)
		{
			WCHAR swp = s[slen];
			s[slen] = s[y];
			s[y++] = swp;
		}

	}
	void __stdcall Reverse() throw()
	{
		StringReverse(m_str);
	}
	GUID __stdcall ToGuid() const
	{
		GUID retval = { 0 };
		HRESULT hr = IIDFromString(m_str, &retval);
		if (FAILED(hr))
			AtlThrow(hr);
		return retval;
	}
	bool __stdcall IsBool() const throw()
	{
		VARIANT_BOOL bogus;
		if (Length() == 0) return false;
		if (lstrcmpiW(L"true", m_str) == 0)	
		{
			return true;
		}
		HRESULT hr = ::VarBoolFromStr(m_str, ::GetThreadLocale(), LOCALE_NOUSEROVERRIDE, &bogus);
		return hr == S_OK;
	}
	bool __stdcall IsNumeric() const throw()
	{
		LONG64 bogus;
		
		HRESULT hr = ::VarI8FromStr(m_str, ::GetThreadLocale(), LOCALE_NOUSEROVERRIDE, &bogus);
		if (hr == S_OK)
		{
			return true;
		}
		DOUBLE bogusr8;
		hr = ::VarR8FromStr(m_str, ::GetThreadLocale(), LOCALE_NOUSEROVERRIDE, &bogusr8);
		
		if (hr == S_OK)
		{
			return true;
		}
		DECIMAL bogusDec;
		hr = ::VarDecFromStr(m_str, ::GetThreadLocale(), LOCALE_NOUSEROVERRIDE, &bogusDec);
		return hr == S_OK ? true : false;
	}

	VARIANT_BOOL __stdcall ToBool() const 
	{
		if (Length() > 0)
		{
			if (lstrcmpiW(L"true", m_str) == 0)
			{
				return VARIANT_TRUE;
			}
			VARIANT_BOOL retVal;
			HRESULT hr = ::VarBoolFromStr(m_str, ::GetThreadLocale(), LOCALE_NOUSEROVERRIDE, &retVal);
			if (FAILED(hr))
			{
				AtlThrow(hr);
			}
			return retVal;
		}
		return VARIANT_FALSE;
	}
	DATE __stdcall ToDate() const
	{
		DATE retval = 0;
		if (Length() > 0)
		{
			HRESULT hr = ::VarDateFromStr(m_str, ::GetThreadLocale(), LOCALE_NOUSEROVERRIDE, &retval);
			if (FAILED(hr))
				AtlThrow(hr);
		}
		return retval;
	}
	_Check_return_ STDMETHODIMP AssignDate(_In_ const DATE date) throw()
	{
		Empty();
		return ::VarBstrFromDate(date, ::GetThreadLocale(), LOCALE_NOUSEROVERRIDE, &m_str);
	}

	_Check_return_ HRESULT AssignBSTR(_In_opt_z_ const BSTR bstrSrc) throw()
	{
		HRESULT hr = S_OK;
		if (m_str != bstrSrc)
		{

			if (bstrSrc != NULL)
			{
				INT len = ::SysStringByteLen(bstrSrc);
				if (len % 2 == 0)
				{
					::SysReAllocStringLen(&m_str, bstrSrc, len / 2);
				}
				else
				{
					::SysFreeString(m_str);
					m_str = ::SysAllocStringByteLen((char*)bstrSrc, len);
				}
				if (!*this)
				{
					hr = E_OUTOFMEMORY;
				}
			}
			else
			{
				Empty();
			}
		}

		return hr;
	}
	//creates a copy to a byteencoded string
	// optimal usage, attach to it...
	BSTR ToByteString(_In_ UINT codePage=CP_UTF8) const
	{
		if (!IsEmpty())
		{	
			UINT neededLen = 0;
			auto len = Length();
			BSTR pszA = NULL;  ;
			//memset(pszA, 0, neededLen);
			SetLastError(0);
			int _convert = WideCharToMultiByte(codePage, 0, m_str, len, (PSTR)pszA, neededLen, NULL, NULL);
			auto err = GetLastError();
			if (_convert > 0) //ERROR_INSUFFICIENT_BUFFER not always set, so do not check
			{
				auto hr = _SetByteLength(&pszA, _convert);
				if (SUCCEEDED(hr))
					_convert = WideCharToMultiByte(codePage, 0, m_str, len, (PSTR) pszA, _convert, NULL, NULL);
				else
					AtlThrowImpl(hr);
				
			}
			err = GetLastError();
			if (_convert == 0 && err > 0)
				AtlThrowImpl(HRESULT_FROM_WIN32(err));
			_SetByteLength(&pszA, _convert);

			return pszA;
		}
		else 
			return NULL;
	}
	//creates a copy to a wide-string
	// optimal usage, attach to it...
	BSTR ToWideString(_In_ UINT codePage=CP_UTF8) throw()
	{
		UINT len = ByteLength();
		int _convert = MultiByteToWideChar(codePage, 0, (PSTR) m_str, len, NULL, 0);
		BSTR pszW = SysAllocStringLen(NULL, _convert);
		if (pszW != NULL && _convert > 0)
		{
			MultiByteToWideChar(codePage, 0, (PSTR)m_str, len, pszW, _convert);
		}
		return pszW;

	}

	_Check_return_ HRESULT ToLower() throw()
	{
		if (m_str != NULL)
		{
#ifdef _UNICODE
			// Convert in place
			CharLowerBuff(m_str, Length());
#else
			// Cannot use conversion macros due to possible embedded NULLs
			UINT _acp = _AtlGetConversionACP();
			int _convert = WideCharToMultiByte(_acp, 0, m_str, Length(), NULL, 0, NULL, NULL);
			CTempBuffer<char> pszA;
			ATLTRY(pszA.Allocate(_convert));
			if (pszA == NULL)
				return E_OUTOFMEMORY;

			int nRet = WideCharToMultiByte(_acp, 0, m_str, Length(), pszA, _convert, NULL, NULL);
			if (nRet == 0)
			{
				ATLASSERT(0);
				return AtlHresultFromLastError();
			}

			CharLowerBuff(pszA, nRet);

			_convert = MultiByteToWideChar(_acp, 0, pszA, nRet, NULL, 0);

			CTempBuffer<WCHAR> pszW;
			ATLTRY(pszW.Allocate(_convert));
			if (pszW == NULL)
				return E_OUTOFMEMORY;

			nRet = MultiByteToWideChar(_acp, 0, pszA, nRet, pszW, _convert);
			if (nRet <= 0)
			{
				ATLASSERT(0);
				return AtlHresultFromLastError();
			}

			UINT nBytes=0;	
			HRESULT hr=S_OK;
			if( FAILED(hr=::ATL::AtlMultiply(&nBytes, static_cast<UINT>(nRet), static_cast<UINT>(sizeof(OLECHAR)))))
			{
				return hr;
			}
			BSTR b = ::SysAllocStringByteLen((LPCSTR) (LPWSTR) pszW, nBytes);
			if (b == NULL)
				return E_OUTOFMEMORY;
			SysFreeString(m_str);
			m_str = b;
#endif
		}
		return S_OK;
	}
	_Check_return_ STDMETHODIMP ToUpper() throw()
	{
		if (m_str != NULL)
		{
#ifdef _UNICODE
			// Convert in place
			CharUpperBuff(m_str, Length());
#else
		// Cannot use conversion macros due to possible embedded NULLs
			UINT _acp = _AtlGetConversionACP();
			int _convert = WideCharToMultiByte(_acp, 0, m_str, Length(), NULL, 0, NULL, NULL);
			CTempBuffer<char> pszA;
			ATLTRY(pszA.Allocate(_convert));
			if (pszA == NULL)
				return E_OUTOFMEMORY;

			int nRet = WideCharToMultiByte(_acp, 0, m_str, Length(), pszA, _convert, NULL, NULL);
			if (nRet == 0)
			{
				ATLASSERT(0);
				return AtlHresultFromLastError();
			}

			CharUpperBuff(pszA, nRet);

			_convert = MultiByteToWideChar(_acp, 0, pszA, nRet, NULL, 0);

			CTempBuffer<WCHAR> pszW;
			ATLTRY(pszW.Allocate(_convert));
			if (pszW == NULL)
				return E_OUTOFMEMORY;

			nRet = MultiByteToWideChar(_acp, 0, pszA, nRet, pszW, _convert);
			if (nRet <= 0)
			{
				ATLASSERT(0);
				return AtlHresultFromLastError();
			}

			UINT nBytes=0;
			HRESULT hr=S_OK;
			if( FAILED(hr=::ATL::AtlMultiply(&nBytes, static_cast<UINT>(nRet), static_cast<UINT>(sizeof(OLECHAR)))))
			{		
				return hr;
			}
			BSTR b = ::SysAllocStringByteLen((LPCSTR) (LPWSTR) pszW, nBytes);
			if (b == NULL)
				return E_OUTOFMEMORY;
			SysFreeString(m_str);
			m_str = b;
#endif
		}
		return S_OK;
	}

	bool __cdecl LoadNString(_In_ UINT uID,  ...) throw()
	{
		va_list args;
		va_start(args, uID);
		bool res = _LoadNString(uID, _AtlBaseModule.GetResourceInstance(), args);
		va_end(args);
		return res;
	}
	bool __cdecl LoadNString2(_In_ UINT uID, _In_ HINSTANCE hInst, ...) throw()
	{
		va_list args;
		va_start(args, hInst);
		bool res = _LoadNString(uID, hInst, args);
		va_end(args);
		return res;
	}
private:
	bool __cdecl _LoadNString(_In_ UINT uID, _In_ HINSTANCE hInst, _In_ va_list args) throw()
	{
		
		bool result = true;			

		PWSTR myMessage = NULL;
		LANGID langid = LANGIDFROMLCID(::GetThreadLocale());
		LANGID id= PRIMARYLANGID(langid);
		int trycnt = 0;
		UINT bufSize =0;
		DWORD dwerr;
		
		::SetLastError(ERROR_SUCCESS); //fix
		do
		{
			trycnt++;
			bufSize = ::FormatMessageW(FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_ALLOCATE_BUFFER,
				hInst, uID, id, (PWSTR) &myMessage, 0, &args);
			
			 dwerr = ::GetLastError();
			 if (dwerr == ERROR_RESOURCE_LANG_NOT_FOUND && trycnt == 1)
			 {
				 id= SUBLANGID(langid);
			 }
			 else if (dwerr == ERROR_RESOURCE_LANG_NOT_FOUND && trycnt == 2)
			 {
				 id = LANG_NEUTRAL;
			 }
		} while (dwerr == ERROR_RESOURCE_LANG_NOT_FOUND && trycnt < 4);

		if (dwerr != ERROR_SUCCESS) 
		{
			AtlTrace(L"_LoadNString %d", dwerr);
			result = false;
		}
		else
		{
			Empty();
			if (bufSize > 0) bufSize -= 2;
			m_str = ::SysAllocStringLen(myMessage, bufSize);
		}
		if (myMessage != NULL) 
			::GlobalFree(myMessage);
	
		return result;
   }
public:


#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
	bool LoadString(
		_In_ HINSTANCE hInst,
		_In_ UINT nID) throw()
	{
		Empty();
		return LoadStringResource(hInst, nID, m_str);
	}

	bool LoadString(_In_ UINT nID) throw()
	{
		Empty();
		return LoadStringResource(nID, m_str);
	}
#endif // _ATL_USE_WINAPI_FAMILY_DESKTOP_APP

	CComBSTR2& __stdcall operator+=(_In_ const CComBSTR2& bstrSrc)
	{
		HRESULT hr;
		hr = AppendBSTR(bstrSrc.m_str);
		if (FAILED(hr))
			AtlThrow(hr);
		return *this;
	}
	CComBSTR2& __stdcall operator+=(_In_opt_ const VARIANT& pVar)
	{
		HRESULT hr = Append(pVar);
		if (FAILED(hr)) 
			AtlThrow(hr);
		return *this;
	}
	CComBSTR2& __stdcall operator+=(_In_opt_ LPCOLESTR pszSrc)
	{
		HRESULT hr = Append(pszSrc);
		if (FAILED(hr))
			AtlThrow(hr);
		return *this;
	}
	
	bool __stdcall operator<(_In_ const CComBSTR2& bstrSrc) const throw()
	{
		return Compare(m_str, bstrSrc.m_str, true, true, true) == -1;
	}
	bool __stdcall operator<(LPCOLESTR pszSrc) const
	{
		CComBSTR2 bstr2(pszSrc);
		return operator<(bstr2);
	}
	bool __stdcall operator<(_In_opt_ LPOLESTR pszSrc) const throw()
	{
		return operator<((LPCOLESTR)pszSrc);
	}

	bool __stdcall operator>(const CComBSTR2& bstrSrc) const throw()
	{
		return Compare(m_str, bstrSrc.m_str, true, true, true) == -1;
	}
	bool operator>(_In_opt_ LPCOLESTR pszSrc) const
	{
		CComBSTR2 bstr2(pszSrc);
		return operator>(bstr2);
	}
	bool __stdcall operator>(_In_opt_ LPOLESTR pszSrc) const throw()
	{
		return operator>((LPCOLESTR)pszSrc);
	}
	
	bool __stdcall operator!=(_In_ const CComBSTR2& bstrSrc) const throw()
	{
		return !operator==(bstrSrc);
	}
	bool __stdcall operator!=(_In_opt_ LPCOLESTR pszSrc) const throw()
	{
		return !operator==(pszSrc);
	}
	bool operator!=(int nNull) const throw()
	{
		return !operator==(nNull);
	}
	bool __stdcall operator!=(_In_z_  LPOLESTR pszSrc) const
	{
		return operator!=((LPCOLESTR)pszSrc);
	}

	bool __stdcall operator==(const CComBSTR2& bstrSrc) const throw()
	{
		return (Compare(m_str, bstrSrc.m_str, true, true, true) == 0);
	}
	bool __stdcall operator==(_In_opt_ LPCOLESTR pszSrc) const throw()
	{
		CComBSTR2 bstr2(pszSrc);
		return operator==(bstr2);
	}
	bool __stdcall operator==(_In_z_ LPOLESTR pszSrc) const throw()
	{
		return operator==((LPCOLESTR)pszSrc);
	}
	
	bool __stdcall operator==(int nNull) const throw()
	{
		ATLASSERT(nNull == NULL);
		(void)nNull;
		return (m_str == NULL);
	}
	CComBSTR2(_In_opt_ LPCSTR pSrc)
	{
		if (pSrc != NULL)
		{
			m_str = A2WBSTR(pSrc);
			if (m_str == NULL)
				AtlThrow(E_OUTOFMEMORY);
		}
		else
			m_str = NULL;
	}

	CComBSTR2(int nSize, _In_opt_count_(nSize) LPCSTR sz)
	{
		if (nSize != 0 && sz == NULL)
		{
			HRESULT hr = SetLength(nSize);
			if (FAILED(hr))
				AtlThrow(hr);
		}
		else 
		{
			m_str = A2WBSTR(sz, nSize);
			if (m_str == NULL && nSize != 0)
				AtlThrow(E_OUTOFMEMORY);
		}
	}

	STDMETHODIMP Append(_In_opt_ LPCSTR lpsz) throw()
	{
		if (lpsz == NULL)
			return S_OK;

		CComBSTR2 bstrTemp;
		ATLTRY(bstrTemp = lpsz);
		if (bstrTemp.m_str == NULL)
			return E_OUTOFMEMORY;
		return Append(bstrTemp);
	}
	
	
	CComBSTR2& __stdcall operator=(_In_ ULONG ulong)
	{
		Empty();
		HRESULT hr = ::VarBstrFromUI4(ulong, ::GetThreadLocale(), LOCALE_NOUSEROVERRIDE, &m_str);
		if (FAILED(hr)) AtlThrow(hr);
		return *this;
	}
	// Define assignment operator.
	//A& A::operator=( const A& );
	ULONG __stdcall ToULong() const
	{
		ULONG retval = 0;
		if (m_str != NULL)
		{
			HRESULT hr = ::VarUI4FromStr(m_str, ::GetThreadLocale(), LOCALE_NOUSEROVERRIDE, &retval);
			if (FAILED(hr))
				AtlThrow(hr);
			}
		return retval;
	}
	DECIMAL __stdcall ToDecimal() const
	{
		DECIMAL retVal = { 0 };
		if (Length() > 0)
		{
			HRESULT hr = ::VarDecFromStr(m_str, ::GetThreadLocale(), LOCALE_NOUSEROVERRIDE, &retVal);
			if (FAILED(hr))
				AtlThrow(hr);
		}
		return retVal;
	}
	LONG __stdcall ToLong() const
	{
		LONG retval = 0;
		if (Length() > 0)
		{
			HRESULT hr = ::VarI4FromStr(m_str, ::GetThreadLocale(), LOCALE_NOUSEROVERRIDE, &retval);
			if (FAILED(hr))
				AtlThrow(hr);
		}
		return retval;
	}
	STDMETHODIMP AssignWordHex(_In_ WORD pVal, _In_ bool fixedLen = true) throw()
	{
		PCWSTR fmt =  fixedLen ? L"%04x" : L"%x";
		return Format(fmt, pVal);
	}
	STDMETHODIMP AssignLongHex(_In_ LONG pVal, _In_ bool fixedLen = true) throw()
	{
		PCWSTR fmt = fixedLen ? 	L"%08x" : L"%x";
		return Format(fmt, pVal) ;		
	}
	CComBSTR2& __stdcall operator=(_In_ LONG pSrc)
	{
		Empty();
		HRESULT hr = ::VarBstrFromI4(pSrc, ::GetThreadLocale(), LOCALE_NOUSEROVERRIDE, &m_str);
		if (FAILED(hr)) AtlThrow(hr);
		return *this;
	}
	CComBSTR2& __stdcall operator =(_In_ DECIMAL pSrc)
	{
		Empty();
		HRESULT hr = ::VarBstrFromDec(&pSrc, ::GetThreadLocale(), LOCALE_NOUSEROVERRIDE, &m_str);
		if (FAILED(hr))
			AtlThrow(hr);
		return *this;
	}
	CComBSTR2& __stdcall operator=(_In_ USHORT pSrc)
	{
		Empty();
		HRESULT hr = ::VarBstrFromUI2(pSrc, ::GetThreadLocale(), LOCALE_NOUSEROVERRIDE, &m_str);
		if (FAILED(hr)) AtlThrow(hr);
		return *this;
	}
	SHORT __stdcall ToShort() const
	{
		SHORT retval = 0;
		if (Length() > 0)
		{
			HRESULT hr = ::VarI2FromStr(m_str, ::GetThreadLocale(), LOCALE_NOUSEROVERRIDE, &retval);
			if (FAILED(hr))
				AtlThrow(hr);
		}
		return retval;
	}
	CComBSTR2& __stdcall operator=(_In_ SHORT pSrc)
	{
		Empty();
		HRESULT hr = ::VarBstrFromI2(pSrc, ::GetThreadLocale(), LOCALE_NOUSEROVERRIDE, &m_str);
		if (FAILED(hr)) AtlThrow(hr);
		return *this;
	}
	BYTE __stdcall ToByte() const
	{
		BYTE retval = 0;
		if (Length() > 0)
		{
			HRESULT hr = ::VarUI1FromStr(m_str, ::GetThreadLocale(), LOCALE_NOUSEROVERRIDE, &retval);
			if (FAILED(hr))
				AtlThrow(hr);
		}
		return retval;
	}
	//cannot use operator = because of VARIANT_BOOL has same type as SHORT
	HRESULT __stdcall FromBool(_In_ VARIANT_BOOL src)
	{
		Empty();
		HRESULT hr = ::VarBstrFromBool(src, ::GetThreadLocale(), LOCALE_NOUSEROVERRIDE, &m_str);
		return hr;
	}
	CComBSTR2& __stdcall operator=(_In_ BYTE pSrc)
	{
		Empty();
		HRESULT hr = ::VarBstrFromUI1(pSrc, ::GetThreadLocale(), LOCALE_NOUSEROVERRIDE, &m_str);
		if (FAILED(hr)) AtlThrow(hr);
		return *this;
	}
#if (_WIN32_WINNT >= 0x0501) || defined(_ATL_SUPPORT_VT_I8)
	CComBSTR2& operator =(LONG64 pSrc)
	{
		Empty();
		HRESULT hr = ::VarBstrFromI8(pSrc, ::GetThreadLocale(), LOCALE_NOUSEROVERRIDE, &m_str);
		if (FAILED(hr)) AtlThrow(hr);
		return *this;
	}
	LONG64 ToVlong() const
	{
		LONG64 retval = 0;
		if (Length() > 0)
		{
			HRESULT hr = ::VarI8FromStr(m_str, ::GetThreadLocale(), LOCALE_NOUSEROVERRIDE, &retval);
			if (FAILED(hr)) AtlThrow(hr);
		}
		return retval;
	}
#endif
	FLOAT __stdcall ToFloat() const
	{
		FLOAT retval = 0;
		if (Length() > 0)
		{
			HRESULT hr = ::VarR4FromStr(m_str, ::GetThreadLocale(), LOCALE_NOUSEROVERRIDE, &retval);
			if (FAILED(hr))
				AtlThrow(hr);
		}
		return retval;
	}
	CComBSTR2& __stdcall operator=(_In_ FLOAT pSrc)
	{
		Empty();
		HRESULT hr = ::VarBstrFromR4(pSrc, ::GetThreadLocale(), LOCALE_NOUSEROVERRIDE, &m_str);
		if (FAILED(hr)) AtlThrow(hr);
		return *this;
	}
	DOUBLE __stdcall ToDouble() const
	{
		DOUBLE retval = 0;
		if (Length() > 0)
		{
			HRESULT hr = ::VarR8FromStr(m_str, ::GetThreadLocale(), LOCALE_NOUSEROVERRIDE, &retval);
			if (FAILED(hr))
				AtlThrow(hr);
		}
		return retval;
	}
	CComBSTR2& __stdcall operator=(_In_ DOUBLE pSrc)
	{
		Empty();
		HRESULT hr = ::VarBstrFromR8(pSrc, ::GetThreadLocale(), LOCALE_NOUSEROVERRIDE, &m_str);
		if (FAILED(hr)) AtlThrow(hr);
		return *this;
	}
	CComBSTR2& __stdcall operator=(_In_opt_ const std::wstring pSrc)
	{
		
		if (!pSrc.empty())
		{
			if (::SysReAllocStringLen(&m_str, pSrc.c_str(), pSrc.length()) == FALSE)
				AtlThrow(E_OUTOFMEMORY);
		}
		else
		{
			Empty();
		}
		return *this;
	}

	CComBSTR2& __stdcall operator=(_In_opt_ const std::string pSrc)
	{
		Empty();
		m_str = A2WBSTR(pSrc.c_str(), pSrc.length());
		if (m_str == NULL && !pSrc.empty())
			AtlThrow(E_OUTOFMEMORY);
		return *this;
	}
	CComBSTR2& __stdcall operator=(_In_opt_ LPCSTR pSrc)
	{
		Empty();
		m_str = A2WBSTR(pSrc);
		if (m_str == NULL && pSrc != NULL)
			AtlThrow(E_OUTOFMEMORY);
		return *this;
	}
	bool __stdcall operator<(_In_opt_ LPCSTR pszSrc) const
	{
		CComBSTR2 bstr2(pszSrc);
		return operator<(bstr2);
	}
	bool __stdcall operator>(_In_opt_ LPCSTR pszSrc) const
	{
		CComBSTR2 bstr2(pszSrc);
		return operator>(bstr2);
	}
	bool __stdcall operator!=(_In_opt_ LPCSTR pszSrc) const
	{
		return !operator==(pszSrc);
	}
	bool __stdcall operator==(_In_opt_ LPCSTR pszSrc) const
	{
		CComBSTR2 bstr2(pszSrc);
		return operator==(bstr2);
	}
	_Check_return_ STDMETHODIMP WriteToStream(_Inout_ IStream* pStream) throw()
	{
		ATLASSERT(pStream != NULL);
		if(pStream == NULL)
			return E_INVALIDARG;
			
		ULONG cb;
		ULONG cbStrLen = CComBSTR::GetStreamSize(m_str) - sizeof(ULONG);
		HRESULT hr = pStream->Write((void*) &cbStrLen, sizeof(cbStrLen), &cb);
		if (FAILED(hr))
			return hr;
		return cbStrLen ? pStream->Write((void*) m_str, cbStrLen, &cb) : S_OK;
	}

	_Check_return_ STDMETHODIMP ReadFromStream(_Inout_ IStream* pStream) throw()
	{
		ATLASSERT(pStream != NULL);
		if(pStream == NULL)
		{
			return E_INVALIDARG;
		}
			
		ATLASSERT(!*this); // should be empty
		Empty();
		
		HRESULT hrSeek;
		ULARGE_INTEGER nBegOffset;
		{
			LARGE_INTEGER nZeroOffset;
			nZeroOffset.QuadPart = 0L;
			hrSeek = pStream->Seek(nZeroOffset, STREAM_SEEK_CUR, &nBegOffset);
		}

		ULONG cbRead = 0;
		ULONG cbStrLen = 0;
		HRESULT hr = pStream->Read(reinterpret_cast<void*>(&cbStrLen), sizeof(cbStrLen), &cbRead);

		if (SUCCEEDED(hr))
		{
			// invalid data size
			if (sizeof(cbStrLen) != cbRead)
			{
				ATLTRACE(atlTraceCOM, 0, _T("Input stream is corrupted."));
				hr = E_FAIL;
			}
			// read NULL string
			else if (cbStrLen == 0)
			{				
			}
			// invalid data length	
			else if (cbStrLen < sizeof(OLECHAR))
			{
				ATLTRACE(atlTraceCOM, 0, _T("Input stream is corrupted."));
				hr = E_FAIL;
			}
			// security checks when system hang for huge stream of data
			else if (cbStrLen > _ATL_STREAM_MAX_SIZE)
			{
				ATLTRACE(atlTraceCOM, 0, _T("String exceeded the maximum allowed size see _ATL_STREAM_MAX_SIZE."));
				hr = E_ACCESSDENIED;
			}
			else 
			{
				//subtract size for terminating NULL which we wrote out
				cbStrLen -= sizeof(OLECHAR);

				m_str = ::SysAllocStringByteLen(NULL, cbStrLen);
				if (!*this)
				{
					hr = E_OUTOFMEMORY;
				}
				else
				{
					hr = pStream->Read(reinterpret_cast<void*>(m_str), cbStrLen, &cbRead);

					if (SUCCEEDED(hr))
					{
						if (cbRead != cbStrLen)
						{
							ATLTRACE(atlTraceCOM, 0, _T("Length of string data is different than expected."));
							hr = E_FAIL;
						}
						else
						{
							OLECHAR ch;
							hr = pStream->Read(reinterpret_cast<void*>(&ch), sizeof(OLECHAR), &cbRead);	

							if (SUCCEEDED(hr))
							{								
								if (cbRead != sizeof(OLECHAR)) 
								{
									ATLTRACE(atlTraceCOM, 0, _T("Cannot read NULL terminator from stream."));
									hr = E_FAIL; 									
								}
								else
								{
									//check if string is properly terminated with NULL
									ATLASSERT(ch == L'\0');
								}
							}
						}
					}			
						
					if (FAILED(hr))
					{
						::SysFreeString(m_str);
						m_str = NULL;
					}
				}
			}
		}
		
		// If SysAllocStringByteLen or IStream::Read failed, reset seek 
		// pointer to start of BSTR size.
		if (FAILED(hr) && SUCCEEDED(hrSeek))
		{
			LARGE_INTEGER nOffset;
			nOffset.QuadPart = static_cast<LONGLONG>(nBegOffset.QuadPart);
			pStream->Seek(nOffset, STREAM_SEEK_SET, NULL);				
		}
	
		return hr;
	}

#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
	_Success_(return != false)
		static bool LoadStringResource(
		_In_ HINSTANCE hInstance,
		_In_ UINT uID,
		_Inout_ _Pre_null_ _Post_z_ BSTR& bstrText) throw()
	{
		ATLASSERT(bstrText == NULL);

		const ATLSTRINGRESOURCEIMAGE* pImage = AtlGetStringResourceImage(hInstance, uID);
		if (pImage != NULL)
		{
			bstrText = ::SysAllocStringLen(pImage->achString, pImage->nLength);
		}
		else
		{
			bstrText = NULL;
		}
		return (bstrText != NULL) ? true : false;
	}

	_Success_(return != false)
		static bool LoadStringResource(
		_In_ UINT uID,
		_Inout_ _Pre_null_ _Post_z_ BSTR& bstrText) throw()
	{
		ATLASSERT(bstrText == NULL);

		const ATLSTRINGRESOURCEIMAGE* pImage = AtlGetStringResourceImage(uID);
		if (pImage != NULL)
		{
			bstrText = ::SysAllocStringLen(pImage->achString, pImage->nLength);
		}
		else
		{
			bstrText = NULL;
		}

		return (bstrText != NULL) ? true : false;
	}

#endif
	// each character in BSTR is copied to each element in SAFEARRAY
	HRESULT BSTRToArray(_Outptr_ LPSAFEARRAY *ppArray) throw()
	{
		return VectorFromBstr(m_str, ppArray);
	}


	// first character of each element in SAFEARRAY is copied to BSTR
	_Check_return_ HRESULT ArrayToBSTR(_In_ const SAFEARRAY *pSrc) throw()
	{
		::SysFreeString(m_str);
		return BstrFromVector((LPSAFEARRAY)pSrc, &m_str);
	}	

};

/////////////////////////////////////////////////////////////
// Class to Adapt CComBSTR2 and CComPtr for use with STL containers
// the syntax to use it is
// std::vector< CAdapt <CComBSTR2> > vect;

/////////////////////////////////////////////////////////////////////////////
// CComVariant2

class CComVariant2 : public tagVARIANT
{
// Constructors
public:
	CComVariant2() throw()
	{
		ZeroMemory(this, sizeof(VARIANT));
	}
	~CComVariant2() throw()
	{
		HRESULT hr = Clear();
		ATLASSERT(SUCCEEDED(hr));
		(hr);
	}

	CComVariant2(_In_ const VARIANT& varSrc) ATLVARIANT_THROW()
	{
		vt = VT_EMPTY;
		InternalCopy(&varSrc);
	}

	CComVariant2(_In_ const CComVariant2& varSrc) ATLVARIANT_THROW()
	{
		vt = VT_EMPTY;
		InternalCopy(&varSrc);
	}
	CComVariant2(_In_z_ LPCOLESTR lpszSrc) ATLVARIANT_THROW()
	{
		vt = VT_EMPTY;
		*this = lpszSrc;
	}

	CComVariant2(_In_z_ LPCSTR lpszSrc) ATLVARIANT_THROW()
	{
		vt = VT_EMPTY;
		*this = lpszSrc;
	}

	CComVariant2(_In_ bool bSrc) throw()
	{
		vt = VT_BOOL;
		boolVal = bSrc ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
	}

	CComVariant2(_In_ int nSrc, _In_ VARTYPE vtSrc = VT_I4) ATLVARIANT_THROW()
	{
		ATLASSERT(vtSrc == VT_I4 || vtSrc == VT_INT);
		if (vtSrc == VT_I4 || vtSrc == VT_INT)
		{
			vt = vtSrc;
			intVal = nSrc;
		}
		else
		{
			vt = VT_ERROR;
			scode = E_INVALIDARG;
#ifndef _ATL_NO_VARIANT_THROW
			AtlThrow(E_INVALIDARG);
#endif			
		}
	}
	CComVariant2(_In_ BYTE nSrc) throw()
	{
		vt = VT_UI1;
		bVal = nSrc;
	}
	CComVariant2(_In_ short nSrc) throw()
	{
		vt = VT_I2;
		iVal = nSrc;
	}
	CComVariant2(_In_ long nSrc, _In_ VARTYPE vtSrc = VT_I4) ATLVARIANT_THROW()
	{
		ATLASSERT(vtSrc == VT_I4 || vtSrc == VT_ERROR);
		if (vtSrc == VT_I4 || vtSrc == VT_ERROR)
		{
			vt = vtSrc;
			lVal = nSrc;
		}
		else
		{
			vt = VT_ERROR;
			scode = E_INVALIDARG;
#ifndef _ATL_NO_VARIANT_THROW
			AtlThrow(E_INVALIDARG);
#endif
		}
	}
	CComVariant2(_In_ float fltSrc) throw()
	{
		vt = VT_R4;
		fltVal = fltSrc;
	}
		CComVariant2(_In_ double dblSrc, _In_ VARTYPE vtSrc = VT_R8) ATLVARIANT_THROW()
	{
		ATLASSERT(vtSrc == VT_R8 || vtSrc == VT_DATE);
		if (vtSrc == VT_R8 || vtSrc == VT_DATE)
		{
			vt = vtSrc;
			dblVal = dblSrc;
		}
		else
		{
			vt = VT_ERROR;
			scode = E_INVALIDARG;
#ifndef _ATL_NO_VARIANT_THROW
			AtlThrow(E_INVALIDARG);
#endif
		}
	}
#if (_WIN32_WINNT >= 0x0501) || defined(_ATL_SUPPORT_VT_I8)
	CComVariant2(_In_ LONG64 nSrc) throw()
	{
		vt = VT_I8;
		llVal = nSrc;
	}
	CComVariant2(ULONGLONG nSrc) throw()
	{
		vt = VT_UI8;
		ullVal = nSrc;
	}
#endif
	// by e.n. works like ADsBuildVarArrayStr
	CComVariant2(__in_ecount_opt(elements) PCWSTR theArray[], int elements) throw()
	{
		BuildVarArray(theArray, elements);
	}
	CComVariant2(CY cySrc) throw()
	{
		vt = VT_CY;
		cyVal.Hi = cySrc.Hi;
		cyVal.Lo = cySrc.Lo;
	}
	CComVariant2(_In_opt_ IDispatch* pSrc) throw()
	{
		vt = VT_DISPATCH;
		pdispVal = pSrc;
		// Need to AddRef as VariantClear will Release
		if (pdispVal != NULL)
			pdispVal->AddRef();
	}
	CComVariant2(_In_opt_ IUnknown* pSrc) throw()
	{
		vt = VT_UNKNOWN;
		punkVal = pSrc;
		// Need to AddRef as VariantClear will Release
		if (punkVal != NULL)
			punkVal->AddRef();
	}
	CComVariant2(_In_ char cSrc) throw()
	{
		vt = VT_I1;
		cVal = cSrc;
	}
	CComVariant2(_In_ unsigned short nSrc) throw()
	{
		vt = VT_UI2;
		uiVal = nSrc;
	}
	CComVariant2(_In_ unsigned long nSrc) throw()
	{
		vt = VT_UI4;
		ulVal = nSrc;
	}
	CComVariant2(_In_ unsigned int nSrc, _In_ VARTYPE vtSrc = VT_UI4) ATLVARIANT_THROW()
	{
		ATLASSERT(vtSrc == VT_UI4 || vtSrc == VT_UINT);
		if (vtSrc == VT_UI4 || vtSrc == VT_UINT)
		{
			vt = vtSrc;
			uintVal= nSrc;
		}
		else
		{
			vt = VT_ERROR;
			scode = E_INVALIDARG;
#ifndef _ATL_NO_VARIANT_THROW
			AtlThrow(E_INVALIDARG);
#endif
		}		
	}
	CComVariant2(_In_ const CComBSTR2& bstrSrc) ATLVARIANT_THROW()
	{
		vt = VT_EMPTY;
		*this = bstrSrc;
	}
	CComVariant2(_In_ const SAFEARRAY *pSrc) ATLVARIANT_THROW()
	{
		ATLASSERT(pSrc != NULL);
		if (pSrc == NULL)
		{
			vt = VT_ERROR;
			scode = E_INVALIDARG;
#ifndef _ATL_NO_VARIANT_THROW
			AtlThrow(E_INVALIDARG);
#endif
		}
		else
		{
			LPSAFEARRAY pCopy;
			HRESULT hRes = ::SafeArrayCopy((LPSAFEARRAY)pSrc, &pCopy);
			if (SUCCEEDED(hRes))
			{
				hRes = ::ATL::AtlSafeArrayGetActualVartype((LPSAFEARRAY)pSrc, &vt);
				if (SUCCEEDED(hRes))
				{
					vt |= VT_ARRAY;
					parray = pCopy;
				}
				else
				{
					vt = VT_ERROR;
					scode = hRes;
				}
			}
			else
			{
				vt = VT_ERROR;
				scode = hRes;
			}

#ifndef _ATL_NO_VARIANT_THROW
			if (FAILED(hRes))
			{
				if (hRes == E_OUTOFMEMORY)
				{
					AtlThrow(E_OUTOFMEMORY);
				}
				else
				{
					ATLENSURE_THROW(FALSE, hRes);
				}
			}
#endif
		}
	}
// Assignment Operators
public:
	CComVariant2& operator=(_In_ const CComVariant2& varSrc) ATLVARIANT_THROW()
	{
		if(this!=&varSrc)
        {
		    InternalCopy(&varSrc);
        }
		return *this;
	}
	CComVariant2& operator=(_In_ const VARIANT& varSrc) ATLVARIANT_THROW()
	{
		if(static_cast<VARIANT *>(this)!=&varSrc)
        {
			InternalCopy(&varSrc);
		}
		return *this;
	}

	CComVariant2& operator=(_In_ const CComBSTR2& bstrSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_BSTR) 
		{
			ClearThrow();
			vt = VT_BSTR;
		}
		if (bstrSrc.m_str == NULL)		
		{
			::SysFreeString(bstrVal);		
			bstrVal = NULL;
			vt = VT_EMPTY;
		}
		else if (::SysReAllocStringLen(&bstrVal, bstrSrc.m_str, bstrSrc.Length()) == FALSE)
		{
			vt = VT_ERROR;
			scode = E_OUTOFMEMORY;
#ifndef _ATL_NO_VARIANT_THROW
			AtlThrow(E_OUTOFMEMORY);
#endif
		}
		return *this;
	}		
	
	CComVariant2& operator=(_In_z_ LPCOLESTR lpszSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_BSTR) 
		{
			ClearThrow();
			vt = VT_BSTR;
		}
		if (::SysReAllocString(&bstrVal, lpszSrc) == FALSE)
		{
			vt = VT_ERROR;
			scode = E_OUTOFMEMORY;
#ifndef _ATL_NO_VARIANT_THROW
			AtlThrow(E_OUTOFMEMORY);
#endif
		}
		return *this;
	}

	CComVariant2& operator=(_In_z_ LPCSTR lpszSrc) ATLVARIANT_THROW()
	{
		USES_CONVERSION_EX;
		if (vt != VT_BSTR) 
		{
			ClearThrow();
			vt = VT_BSTR;
		}		
		if (::SysReAllocString(&bstrVal, A2COLE_EX(lpszSrc, _ATL_SAFE_ALLOCA_DEF_THRESHOLD)) == FALSE)
		{
			vt = VT_ERROR;
			scode = E_OUTOFMEMORY;
#ifndef _ATL_NO_VARIANT_THROW
			AtlThrow(E_OUTOFMEMORY);
#endif

		}
		return *this;
	}

	CComVariant2& operator=(_In_ bool bSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_BOOL)
		{
			ClearThrow();
			vt = VT_BOOL;
		}
		boolVal = bSrc ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
		return *this;
	}

	CComVariant2& operator=(int nSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_I4)
		{
			ClearThrow();
			vt = VT_I4;
		}
		intVal = nSrc;

		return *this;
	}

	CComVariant2& operator=(_In_ BYTE nSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_UI1)
		{
			ClearThrow();
			vt = VT_UI1;
		}
		bVal = nSrc;
		return *this;
	}

	CComVariant2& operator=(_In_ short nSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_I2)
		{
			ClearThrow();
			vt = VT_I2;
		}
		iVal = nSrc;
		return *this;
	}

	CComVariant2& operator=(_In_ long nSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_I4)
		{
			ClearThrow();
			vt = VT_I4;
		}
		lVal = nSrc;
		return *this;
	}

	CComVariant2& operator=(_In_ float fltSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_R4)
		{
			ClearThrow();
			vt = VT_R4;
		}
		fltVal = fltSrc;
		return *this;
	}

	CComVariant2& operator=(_In_ double dblSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_R8)
		{
			ClearThrow();
			vt = VT_R8;
		}
		dblVal = dblSrc;
		return *this;
	}

	CComVariant2& operator=(_In_ CY cySrc) ATLVARIANT_THROW()
	{
		if (vt != VT_CY)
		{
			ClearThrow();
			vt = VT_CY;
		}
		cyVal.Hi = cySrc.Hi;
		cyVal.Lo = cySrc.Lo;
		return *this;
	}

	CComVariant2& operator=(_In_opt_ IDispatch* pSrc) ATLVARIANT_THROW()
	{
		ClearThrow();

		vt = VT_DISPATCH;
		pdispVal = pSrc;
		// Need to AddRef as VariantClear will Release
		if (pdispVal != NULL)
			pdispVal->AddRef();
		return *this;
	}

	CComVariant2& operator=(_In_opt_ IUnknown* pSrc) ATLVARIANT_THROW()
	{
		ClearThrow();
		vt = VT_UNKNOWN;
		punkVal = pSrc;

		// Need to AddRef as VariantClear will Release
		if (punkVal != NULL)
			punkVal->AddRef();		
		return *this;
	}

	CComVariant2& operator=(char cSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_I1)
		{
			ClearThrow();
			vt = VT_I1;
		}
		cVal = cSrc;
		return *this;
	}

	CComVariant2& operator=(_In_ unsigned short nSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_UI2)
		{
			ClearThrow();
			vt = VT_UI2;
		}
		uiVal = nSrc;
		return *this;
	}

	CComVariant2& operator=(_In_ unsigned long nSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_UI4)
		{
			ClearThrow();
			vt = VT_UI4;
		}
		ulVal = nSrc;
		return *this;
	}

	CComVariant2& operator=(_In_ unsigned int nSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_UI4)
		{
			ClearThrow();
			vt = VT_UI4;
		}
		uintVal= nSrc;
		return *this;
	}

	CComVariant2& operator=(_In_ BYTE* pbSrc) ATLVARIANT_THROW()
	{
		if (vt != (VT_UI1|VT_BYREF))
		{
			ClearThrow();
			vt = VT_UI1|VT_BYREF;
		}
		pbVal = pbSrc;
		return *this;
	}

	CComVariant2& operator=(_In_ short* pnSrc) ATLVARIANT_THROW()
	{
		if (vt != (VT_I2|VT_BYREF))
		{
			ClearThrow();
			vt = VT_I2|VT_BYREF;
		}
		piVal = pnSrc;
		return *this;
	}

#ifdef _NATIVE_WCHAR_T_DEFINED
	CComVariant2& operator=(_In_ USHORT* pnSrc) ATLVARIANT_THROW()
	{
		if (vt != (VT_UI2|VT_BYREF))
		{
			ClearThrow();
			vt = VT_UI2|VT_BYREF;
		}
		puiVal = pnSrc;
		return *this;
	}
#endif

	CComVariant2& operator=(_In_ int* pnSrc) ATLVARIANT_THROW()
	{
		if (vt != (VT_I4|VT_BYREF))
		{
			ClearThrow();
			vt = VT_I4|VT_BYREF;
		}
		pintVal = pnSrc;
		return *this;
	}

	CComVariant2& operator=(_In_ UINT* pnSrc) ATLVARIANT_THROW()
	{
		if (vt != (VT_UI4|VT_BYREF))
		{
			ClearThrow();
			vt = VT_UI4|VT_BYREF;
		}
		puintVal = pnSrc;
		return *this;
	}

	CComVariant2& operator=(_In_ long* pnSrc) ATLVARIANT_THROW()
	{
		if (vt != (VT_I4|VT_BYREF))
		{
			ClearThrow();
			vt = VT_I4|VT_BYREF;
		}
		plVal = pnSrc;
		return *this;
	}

	CComVariant2& operator=(_In_ ULONG* pnSrc) ATLVARIANT_THROW()
	{
		if (vt != (VT_UI4|VT_BYREF))
		{
			ClearThrow();
			vt = VT_UI4|VT_BYREF;
		}
		pulVal = pnSrc;
		return *this;
	}


	CComVariant2& operator=(_In_ LONGLONG nSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_I8)
		{
			ClearThrow();
			vt = VT_I8;
		}
		llVal = nSrc;

		return *this;
	}

	CComVariant2& operator=(_In_ LONGLONG* pnSrc) ATLVARIANT_THROW()
	{
		if (vt != (VT_I8|VT_BYREF))
		{
			ClearThrow();
			vt = VT_I8|VT_BYREF;
		}
		pllVal = pnSrc;
		return *this;
	}

	CComVariant2& operator=(_In_ ULONGLONG nSrc) ATLVARIANT_THROW()
	{
		if (vt != VT_UI8)
		{
			ClearThrow();
			vt = VT_UI8;
		}
		ullVal = nSrc;

		return *this;
	}

	CComVariant2& operator=(_In_ ULONGLONG* pnSrc) ATLVARIANT_THROW()
	{
		if (vt != (VT_UI8|VT_BYREF))
		{
			ClearThrow();
			vt = VT_UI8|VT_BYREF;
		}
		pullVal = pnSrc;
		return *this;
	}


	CComVariant2& operator=(_In_ float* pfSrc) throw()
	{
		if (vt != (VT_R4|VT_BYREF))
		{
			Clear();
			vt = VT_R4|VT_BYREF;
		}
		pfltVal = pfSrc;
		return *this;
	}

	CComVariant2& operator=(_In_ double* pfSrc) throw()
	{
		if (vt != (VT_R8|VT_BYREF))
		{
			Clear();
			vt = VT_R8|VT_BYREF;
		}
		pdblVal = pfSrc;
		return *this;
	}

	CComVariant2& operator=(_In_ const SAFEARRAY *pSrc) ATLVARIANT_THROW()
	{
		ATLASSERT(pSrc != NULL);

		if (pSrc == NULL)
		{
			ClearThrow();
			vt = VT_ERROR;
			scode = E_INVALIDARG;
#ifndef _ATL_NO_VARIANT_THROW
			AtlThrow(E_INVALIDARG);
#endif
		}
		else if ((vt & VT_ARRAY) == 0 || pSrc != parray)
		{
			ClearThrow();
			LPSAFEARRAY pCopy;
			HRESULT hr = ::SafeArrayCopy((LPSAFEARRAY)pSrc, &pCopy);
			if (SUCCEEDED(hr))
			{
				::ATL::AtlSafeArrayGetActualVartype((LPSAFEARRAY)pSrc, &vt);
				vt |= VT_ARRAY;
				parray = pCopy;
			}
			else
			{
				vt = VT_ERROR;
				scode = hr;
#ifndef _ATL_NO_VARIANT_THROW
				if (hr == E_OUTOFMEMORY)
				{
					AtlThrow(E_OUTOFMEMORY);
				}
				else
				{
					ATLENSURE_THROW(FALSE, hr);
				}
#endif
			}
		}

		return *this;
	}


// Comparison Operators
public:
	bool operator==(_In_ const VARIANT& varSrc) const throw()
	{
		// For backwards compatibility
		if (vt == VT_NULL && varSrc.vt == VT_NULL)
		{
			return true;
		}
		// Variants not equal if types don't match
		if (vt != varSrc.vt)
		{
			return false;
		}
		return VarCmp((VARIANT*)this, (VARIANT*)&varSrc, LOCALE_USER_DEFAULT, 0) == static_cast<HRESULT>(VARCMP_EQ);
	}

	bool operator!=(_In_ const VARIANT& varSrc) const throw()
	{
		return !operator==(varSrc);
	}

	bool operator<(_In_ const VARIANT& varSrc) const throw()
	{
		if (vt == VT_NULL && varSrc.vt == VT_NULL)
			return false;
		return VarCmp((VARIANT*)this, (VARIANT*)&varSrc, ::GetThreadLocale(), 0)==VARCMP_LT;
	}

	bool operator>(_In_ const VARIANT& varSrc) const throw()
	{
		if (vt == VT_NULL && varSrc.vt == VT_NULL)
			return false;
		return VarCmp((VARIANT*)this, (VARIANT*)&varSrc, ::GetThreadLocale(), 0)==VARCMP_GT;
	}
private:
	inline HRESULT VarCmp(
		_In_ LPVARIANT pvarLeft, 
		_In_ LPVARIANT pvarRight, 
		_In_ LCID lcid, 
		_In_ ULONG dwFlags) const throw();

// Operations
public:
	HRESULT Clear()
	{ 
		return ClearToZero();		
	}
	HRESULT ClearToZero() 
	{
		HRESULT hr = ::VariantClear(this); 
		if( FAILED(hr) )
		{
			return hr;
		}
		memset(this ,0 ,sizeof(tagVARIANT));

		return hr;
	}

	// behaves like ADsBuildVarArrayStr 
	// returns: VT_ARRAY | VT_VARIANT (BSTR)
	HRESULT BuildVarArray(__in_ecount_opt(elements) VARIANT theArray[], int elements) 
	{
		HRESULT hr = S_OK;
		if (elements <= 0 || theArray == NULL)
			hr = E_INVALIDARG;
		else
		{
			SAFEARRAYBOUND rgsa = {(ULONG)elements, 0};
			SAFEARRAY *psa = SafeArrayCreate(VT_VARIANT, 1, &rgsa);
			if (psa == NULL)
				hr = E_OUTOFMEMORY;
			else
			{
				Clear();
				parray = psa;
				vt = VT_ARRAY | VT_VARIANT;
				VARIANT* copydata;
				hr = SafeArrayLock(psa);
				if (SUCCEEDED(hr))
				{
					while(elements-- != 0)	
					{
						if (SafeArrayPtrOfIndex(psa, (LONG*)&elements, (void**)&copydata) == S_OK)
							VariantCopy(copydata, &theArray[elements]);
					}
					SafeArrayUnlock(psa);
				}
			}
		}
		return hr;
	}
	// behaves like ADsBuildVarArrayStr 
	// returns: VT_ARRAY | VT_VARIANT (BSTR)
	HRESULT BuildVarArray(__in_ecount_opt(elements) PCWSTR theArray[], int elements) throw()
	{
		HRESULT hr = S_OK;
		if (elements <= 0 || theArray == NULL)
			hr = E_INVALIDARG;
		else
		{
			SAFEARRAYBOUND rgsa = {(ULONG)elements, 0};
			SAFEARRAY *psa = SafeArrayCreate(VT_VARIANT, 1, &rgsa);
			if (psa == NULL)
				hr = E_OUTOFMEMORY;
			else
			{
				Clear();
				parray = psa;
				vt = VT_ARRAY | VT_VARIANT;
				VARIANT* copydata;
				hr = SafeArrayAccessData(psa, (void**)&copydata);
				if (SUCCEEDED(hr))
				{
					while(elements-- != 0)	
					{
						copydata[elements].vt = VT_BSTR;
						copydata[elements].bstrVal = SysAllocString(theArray[elements]);
					}				
					SafeArrayUnaccessData(psa);
				}
			}
		}
		return hr;
	}
	HRESULT Copy(_In_ const VARIANT* pSrc) 
	{
		return ::VariantCopy(this, const_cast<VARIANT*>(pSrc)); 
	}
	HRESULT CopyTo(_Out_ VARIANT* pDest) throw()
	{
		return ::VariantCopy(pDest, this);
	}
	HRESULT CopyTo(_Outptr_result_z_ BSTR *pstrDest) const
	{
		ATLASSERT(pstrDest != NULL && vt == VT_BSTR);
		HRESULT hRes = E_POINTER;
		if (pstrDest != NULL && vt == VT_BSTR)
		{
			*pstrDest = ::SysAllocStringByteLen((char*)bstrVal, ::SysStringByteLen(bstrVal));
			if (*pstrDest == NULL)
				hRes = E_OUTOFMEMORY;
			else
				hRes = S_OK;
		}
		else if (vt != VT_BSTR)
			hRes = DISP_E_TYPEMISMATCH;

		return hRes;
	}

	//Added by e.n.
	//attaches to a BSTR
	// clears its contents first, then copies pointer, then assigns NULL 
	// to your pSrc
	HRESULT Attach(_In_ BSTR* pSrc)
	{
		if(pSrc == NULL)
			return E_INVALIDARG;
			
		// Clear out the variant
		HRESULT hr = Clear();
		if (SUCCEEDED(hr) && this->bstrVal != *pSrc)
		{
			// Copy the pointer and give control to CComVariant2
			bstrVal = *pSrc;
			vt = VT_BSTR;

			*pSrc = NULL;
			hr = S_OK;
		}
		return hr;
	}
	HRESULT Attach(_In_ VARIANT* pSrc) throw()
	{
		if(pSrc == NULL)
			return E_INVALIDARG;
			
		HRESULT hr = S_OK;
		if (this != pSrc)
		{
			// Clear out the variant
			hr = Clear();
			if (SUCCEEDED(hr))
			{
				// Copy the contents and give control to CComVariant
				Checked::memcpy_s(this, sizeof(CComVariant), pSrc, sizeof(VARIANT));
				pSrc->vt = VT_EMPTY;
				hr = S_OK;
			}
		}
		return hr;
	}

	//Added by E.N.
	void Detach() throw()
	{
		ZeroMemory(this, sizeof(VARIANT));		
	}
	HRESULT Detach(_Inout_ BSTR* pDest) throw()
	{
		ATLASSERT(pDest != NULL);
		if (*pDest != NULL) 
			SysFreeString(*pDest);
		HRESULT hr = S_OK;
		if (vt != VT_BSTR)
			hr = ChangeType(VT_BSTR);
		*pDest = bstrVal;
		vt = VT_EMPTY;
		bstrVal = NULL;
		return hr;
	}
	HRESULT Detach(_Inout_ VARIANT* pDest)
	{
		ATLASSERT(pDest != NULL);
		if (pDest == NULL)
			return E_POINTER;

		// Clear out the variant
		HRESULT hr = ::VariantClear(pDest);
		if (SUCCEEDED(hr))
		{
			// Copy the contents and remove control from CComVariant
			Checked::memcpy_s(pDest, sizeof(VARIANT), this, sizeof(VARIANT));
			vt = VT_EMPTY;
			hr = S_OK;
		}
		return hr;
	}

	HRESULT ChangeType(_In_ VARTYPE vtNew, _In_ const VARIANT* pSrc = NULL) 
	{
		// Convert in place if pSrc is NULL
		const VARIANT* pVar = const_cast<VARIANT*>(pSrc);
		// Convert in place if pSrc is NULL
		if (pVar == NULL)
			pVar = this;
		// Do nothing if doing in place convert and vts not different change by E.N.
		return ::VariantChangeTypeEx(this, pVar, ::GetThreadLocale(), 0, vtNew);
		//return ::VariantChangeTypeEx(this, pVar, 0, vtNew);
	}

	template< typename T >
	void SetByRef(_In_ T* pT) ATLVARIANT_THROW()
	{
		ClearThrow();
		vt = CVarTypeInfo< T* >::VT;
		byref = pT;
	}
#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
	_Check_return_ HRESULT WriteToStream(_Inout_ IStream* pStream);
	_Check_return_ HRESULT WriteToStream(
		_Inout_ IStream* pStream,
		_In_ VARTYPE vtWrite)
	{
		if (vtWrite != VT_EMPTY && vtWrite != vt)
		{
			CComVariant varConv;
			HRESULT hr = varConv.ChangeType(vtWrite, this);
			if (FAILED(hr))
			{
				return hr;
			}
			return varConv.WriteToStream(pStream);
		}
		return WriteToStream(pStream);
	}

	_Check_return_ HRESULT ReadFromStream(
		_Inout_ IStream* pStream,
		_In_ VARTYPE vtExpected = VT_EMPTY);

	_Check_return_ HRESULT ReadFromStream(
		_Inout_ IStream* pStream,
		_In_ VARTYPE vtExpected,
		_In_ ClassesAllowedInStream rgclsidAllowed,
		_In_ DWORD cclsidAllowed);

	// Return the size in bytes of the current contents
	ULONG GetSize() const;
	HRESULT GetSizeMax(_Out_ ULARGE_INTEGER* pcbSize) const;
#endif // _ATL_USE_WINAPI_FAMILY_DESKTOP_APP

// Implementation
private:
	void ClearThrow() ATLVARIANT_THROW()
	{
		HRESULT hr = Clear();
		ATLASSERT(SUCCEEDED(hr));
		(hr);
#ifndef _ATL_NO_VARIANT_THROW
		if (FAILED(hr))
		{
			AtlThrow(hr);
		}
#endif
	}
public:
	HRESULT InternalClear() ATLVARIANT_THROW()
	{
		HRESULT hr = Clear();
		ATLASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
		{
			vt = VT_ERROR;
			scode = hr;
#ifndef _ATL_NO_VARIANT_THROW
			AtlThrow(hr);
#endif
		}
		return hr;
	}

	void InternalCopy(const VARIANT* pSrc) ATLVARIANT_THROW()
	{
		HRESULT hr = Copy(pSrc);
		if (FAILED(hr))
		{
			vt = VT_ERROR;
			scode = hr;
#ifndef _ATL_NO_VARIANT_THROW
			AtlThrow(hr);
#endif
		}
	}
};
#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
#pragma warning(push)
#pragma warning(disable: 4702)
_Check_return_ inline HRESULT CComVariant2::WriteToStream(_Inout_ IStream* pStream) throw()
{
	if(pStream == NULL)
		return E_INVALIDARG;
		
	HRESULT hr = pStream->Write(&vt, sizeof(VARTYPE), NULL);
	if (FAILED(hr))
		return hr;

	int cbWrite = 0;
	switch (vt)
	{
	case VT_UNKNOWN:
	case VT_DISPATCH:
		{
			CComPtr<IPersistStream> spStream;
			if (punkVal != NULL)
			{
				hr = punkVal->QueryInterface(__uuidof(IPersistStream), (void**)&spStream);
				if (FAILED(hr))
				{
					hr = punkVal->QueryInterface(__uuidof(IPersistStreamInit), (void**)&spStream);
					if (FAILED(hr))
						return hr;
				}
			}
			if (spStream != NULL)
				return OleSaveToStream(spStream, pStream);
			return pStream->Write(&CLSID_NULL, sizeof(GUID), nullptr);
		}
	case VT_UI1:
	case VT_I1:
		cbWrite = sizeof(BYTE);
		break;
	case VT_I2:
	case VT_UI2:
	case VT_BOOL:
		cbWrite = sizeof(short);
		break;
	case VT_I4:
	case VT_UI4:
	case VT_R4:
	case VT_INT:
	case VT_UINT:
	case VT_ERROR:
		cbWrite = sizeof(long);
		break;
	case VT_I8:
	case VT_UI8:
		cbWrite = sizeof(LONGLONG);
		break;
	case VT_R8:
	case VT_CY:
	case VT_DATE:
		cbWrite = sizeof(double);
		break;
	default:
		break;
	}
	if (cbWrite != 0)
		return pStream->Write((void*) &bVal, cbWrite, NULL);

	CComBSTR bstrWrite;
	CComVariant varBSTR;
	if (vt != VT_BSTR)
	{
		hr = VariantChangeType(&varBSTR, this, VARIANT_NOVALUEPROP, VT_BSTR);
		if (FAILED(hr))
			return hr;
		bstrWrite.Attach(varBSTR.bstrVal);
	}
	else
		bstrWrite.Attach(bstrVal);

	hr = bstrWrite.WriteToStream(pStream);
	bstrWrite.Detach();
	return hr;
}
#pragma warning(pop)	// C4702

_Check_return_ inline HRESULT CComVariant::ReadFromStream(
	_Inout_ IStream* pStream,
	_In_ VARTYPE vtExpected,
	_In_ ClassesAllowedInStream rgclsidAllowed,
	_In_ DWORD cclsidAllowed)
{
	ATLASSERT(pStream != NULL);
	if (pStream == NULL)
		return E_INVALIDARG;

	HRESULT hr;
	hr = VariantClear(this);
	if (FAILED(hr))
		return hr;
	VARTYPE vtRead = VT_EMPTY;
	ULONG cbRead = 0;

	hr = pStream->Read(&vtRead, sizeof(VARTYPE), &cbRead);
	ATLPREFAST_SUPPRESS(6102)
		if (hr == S_FALSE || (cbRead != sizeof(VARTYPE) && hr == S_OK))
			hr = E_FAIL;
	ATLPREFAST_UNSUPPRESS()
		if (FAILED(hr))
			return hr;
	if (vtExpected != VT_EMPTY && vtRead != vtExpected)
		return E_FAIL;

	vt = vtRead;
	cbRead = 0;
	switch (vtRead)
	{
	case VT_UNKNOWN:
	case VT_DISPATCH:
	{
		punkVal = NULL;
		hr = AtlInternalOleLoadFromStream(pStream,
			(vtRead == VT_UNKNOWN) ? __uuidof(IUnknown) : __uuidof(IDispatch),
			(void**)&punkVal, rgclsidAllowed, cclsidAllowed);
		// If IPictureDisp or IFontDisp property is not set, 
		// OleLoadFromStream() will return REGDB_E_CLASSNOTREG.
		if (hr == REGDB_E_CLASSNOTREG)
			hr = S_OK;
		return hr;
	}
	case VT_UI1:
	case VT_I1:
		cbRead = sizeof(BYTE);
		break;
	case VT_I2:
	case VT_UI2:
	case VT_BOOL:
		cbRead = sizeof(short);
		break;
	case VT_I4:
	case VT_UI4:
	case VT_R4:
	case VT_INT:
	case VT_UINT:
	case VT_ERROR:
		cbRead = sizeof(long);
		break;
	case VT_I8:
	case VT_UI8:
		cbRead = sizeof(LONGLONG);
		break;
	case VT_R8:
	case VT_CY:
	case VT_DATE:
		cbRead = sizeof(double);
		break;
	default:
		break;
	}
	if (cbRead != 0)
	{
		hr = pStream->Read((void*)&bVal, cbRead, NULL);
		if (hr == S_FALSE)
			hr = E_FAIL;
		return hr;
	}
	CComBSTR bstrRead;

	hr = bstrRead.ReadFromStream(pStream);
	if (FAILED(hr))
	{
		// If CComBSTR::ReadFromStream failed, reset seek pointer to start of
		// variant type.
		LARGE_INTEGER nOffset;
		nOffset.QuadPart = -(static_cast<LONGLONG>(sizeof(VARTYPE)));
		pStream->Seek(nOffset, STREAM_SEEK_CUR, NULL);
		vt = VT_EMPTY;
		return hr;
	}
	vt = VT_BSTR;
	bstrVal = bstrRead.Detach();
	if (vtRead != VT_BSTR)
		hr = ChangeType(vtRead);
	return hr;
}

inline HRESULT CComVariant2::GetSizeMax(_Out_ ULARGE_INTEGER* pcbSize) const
{
	ATLASSERT(pcbSize != NULL);
	if (pcbSize == NULL)
	{
		return E_INVALIDARG;
	}
	
	HRESULT hr = S_OK;
	ULARGE_INTEGER nSize;
	nSize.QuadPart = sizeof(VARTYPE);	
	
	switch (vt)
	{
	case VT_UNKNOWN:
	case VT_DISPATCH:
		{	
			nSize.LowPart += sizeof(CLSID);
			
			if (punkVal != NULL)
			{
				CComPtr<IPersistStream> spStream;
				
				hr = punkVal->QueryInterface(__uuidof(IPersistStream), (void**)&spStream);
				if (FAILED(hr))
				{
					hr = punkVal->QueryInterface(__uuidof(IPersistStreamInit), (void**)&spStream);
					if (FAILED(hr))
					{
						break;
					}
				}
				
				ULARGE_INTEGER nPersistSize;
				nPersistSize.QuadPart = 0;
				
				ATLASSERT(spStream != NULL);
				hr = spStream->GetSizeMax(&nPersistSize);				
				if (SUCCEEDED(hr))
				{
					hr = AtlAdd(&nSize.QuadPart, nSize.QuadPart, nPersistSize.QuadPart);
				}				
			}			
		}
		break;
	case VT_UI1:
	case VT_I1:
		nSize.LowPart += sizeof(BYTE);
		break;
	case VT_I2:
	case VT_UI2:
	case VT_BOOL:
		nSize.LowPart += sizeof(short);
		break;
	case VT_I4:
	case VT_UI4:
	case VT_R4:
	case VT_INT:
	case VT_UINT:
	case VT_ERROR:
		nSize.LowPart += sizeof(long);
		break;
	case VT_I8:
	case VT_UI8:
		nSize.LowPart += sizeof(LONGLONG);
		break;
	case VT_R8:
	case VT_CY:
	case VT_DATE:
		nSize.LowPart += sizeof(double);
		break;
	default:
		{
			VARTYPE vtTmp = vt;
			BSTR bstr = NULL;
			CComVariant varBSTR;
			if (vtTmp != VT_BSTR)
			{
				hr = VariantChangeType(&varBSTR, const_cast<VARIANT*>((const VARIANT*)this), VARIANT_NOVALUEPROP, VT_BSTR);
				if (SUCCEEDED(hr))
				{
					bstr = varBSTR.bstrVal;
					vtTmp = VT_BSTR;
				}
			} 
			else
			{
				bstr = bstrVal;
			}

			if (vtTmp == VT_BSTR)
			{
				// Add the size of the length + string (in bytes) + NULL terminator.				
				nSize.QuadPart += CComBSTR::GetStreamSize(bstr);
			}
		}		
	}
	
	if (SUCCEEDED(hr))
	{
		pcbSize->QuadPart = nSize.QuadPart;
	}
	
	return hr;
}



#endif // _ATL_USE_WINAPI_FAMILY_DESKTOP_APP

/*
	Workaround for VarCmp function which does not compare VT_I1, VT_UI2, VT_UI4, VT_UI8 values
*/
inline HRESULT CComVariant::VarCmp(
	_In_ LPVARIANT pvarLeft, 
	_In_ LPVARIANT pvarRight, 
	_In_ LCID lcid, 
	_In_ ULONG dwFlags) const throw()
{			
	switch(vt) 
	{
		case VT_I1:
			if (pvarLeft->cVal == pvarRight->cVal)
			{
				return VARCMP_EQ;
			}
			return pvarLeft->cVal > pvarRight->cVal ? VARCMP_GT : VARCMP_LT;			
		case VT_UI2:
			if (pvarLeft->uiVal == pvarRight->uiVal)
			{
				return VARCMP_EQ;
			}
			return pvarLeft->uiVal > pvarRight->uiVal ? VARCMP_GT : VARCMP_LT;

		case VT_UI4:
			if (pvarLeft->uintVal == pvarRight->uintVal) 
			{
				return VARCMP_EQ;
			}
			return pvarLeft->uintVal > pvarRight->uintVal ? VARCMP_GT : VARCMP_LT;				

		case VT_UI8:
			if (pvarLeft->ullVal == pvarRight->ullVal)
			{
				return VARCMP_EQ;
			}
			return pvarLeft->ullVal > pvarRight->ullVal ? VARCMP_GT : VARCMP_LT;

		default:
			return ::VarCmp(pvarLeft, pvarRight, lcid, dwFlags);
	}
}



} //namespace ATL /hack