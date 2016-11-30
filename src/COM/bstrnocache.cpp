// compile this with option 'not using precompiled headers'
#include "bstrnocache.h"
// see license in bstrnocache.h
void __stdcall FreeString(BSTR * theString) throw()
{
	if (theString != NULL && *theString != NULL )
	{
		SysFreeString(*theString);
		*theString = NULL;
	}
}

//int __stdcall SysReAllocStringByteLen2(BSTR * bstr, PCSTR input, UINT cch) throw()
//{
////	ATLASSERT(bstr != NULL);
//	UINT_PTR copyIntoLen = input != NULL ?  strlen(input): 0;
//	if (copyIntoLen > cch) 
//		copyIntoLen = cch;
//	UINT_PTR newlen = cch + sizeof(OLECHAR) + sizeof(UINT_PTR);
//	if (newlen > MAXBSTRLEN)
//		return FALSE;	
//	PSTR temp = (PSTR)::CoTaskMemRealloc( 
//									*bstr == NULL ? NULL : (PSTR)(*bstr) - sizeof(UINT_PTR)
//								, MEMALIGN_16(newlen));	
//	if (temp != NULL)
//	{
//		UINT_PTR* plen = (UINT_PTR*)temp;
//		*bstr = (BSTR)&plen[1];
//		UINT* plen2 = (UINT*)temp;
//		plen2[sizePtrCorrection] = cch ; //asign bytelength for BSTR
//		if (copyIntoLen > 0)	//copy values
//			strncpy_s((PSTR)*bstr, cch + 1, input, copyIntoLen);
//	
//		temp[sizeof(UINT_PTR) + cch ] = 0;	//terminate LPWSTR with a wide_string
//		temp[sizeof(UINT_PTR) + cch + 1] = 0;		
//	}
//	
//	return *bstr == NULL ? FALSE : TRUE;
//}
//
//BSTR __stdcall SysAllocStringByteLen2(PCSTR input, UINT cch)  throw()
//{
//	UINT_PTR copyIntoLen = input != NULL ?  strlen(input) : 0;
//	if (copyIntoLen > cch) 
//		copyIntoLen = cch;
//	BSTR retval = NULL;
//	UINT_PTR newlen = cch + sizeof(OLECHAR) + sizeof(UINT_PTR);
//	if (newlen > MAXBSTRLEN)
//		return NULL;
//	PWSTR temp = (PWSTR)::CoTaskMemAlloc(MEMALIGN_16(newlen));
//	if (temp != NULL)
//	{
//		UINT_PTR* plen = (UINT_PTR*)temp;
//		retval = (BSTR)&plen[1];
//		UINT* plen2 = (UINT*)temp;		
//		plen2[sizePtrCorrection] = cch;		
//		if (copyIntoLen > 0)			
//			strncpy_s((PSTR)retval, cch + 1, input, copyIntoLen);
//		
//		PSTR zeroit = (PSTR)temp;
//		zeroit[sizeof(UINT_PTR) + cch ] = 0;	//terminate LPWSTR with a wide_string
//		zeroit[sizeof(UINT_PTR) + cch + 1] = 0;		
//	}
//	return retval;
//}


HRESULT __stdcall AllocPSTR(PSTR *lpMulti, SIZE_T lSize, SIZE_T *oldSize) throw()
{
//	ATLASSERT(lpMulti != NULL && oldSize != NULL);

	HRESULT hr = S_OK;
	if (*oldSize < lSize) 
	{
		PSTR temp = *lpMulti == NULL ? (PSTR)calloc(lSize, 1) : (PSTR)_recalloc(*lpMulti, lSize, 1);
		if (temp == NULL) 	
		{	
			free(*lpMulti);//(h, 0, *lpMulti);
			*lpMulti = NULL;
			*oldSize = 0;
			hr =E_OUTOFMEMORY;
		}
		else
		{
			*lpMulti = temp;//successfull realloc
			*oldSize = lSize;
		}
	}
	return hr;
}
void __stdcall FreeMulti(PSTR *lpMultiString) throw()
{
	if (*lpMultiString != NULL)
	{
		free(*lpMultiString);
		*lpMultiString = NULL;
	}
}
//UINT __stdcall SysStringByteLen2(BSTR theString) throw()
//{
//	//UINT bogus = *(((UINT*)theString) -2 );
//	return theString != NULL ? *(((UINT*)theString) -1 ) : 0;
//}
//
//UINT __stdcall SysStringLen2(BSTR theString) throw()
//{
//	//UINT bogus = *(((UINT*)theString) -2 );
//	return theString != NULL ? (*(((UINT*)theString) -1 )) / sizeof(OLECHAR) : 0;
//}
//void __stdcall SysFreeString2(BSTR theString) throw()
//{
//	//ATLASSERT(theString != NULL);
//	if (theString != NULL)
//	{
//		UINT_PTR* temp = (UINT_PTR*)theString;
//		temp--;
//		CoTaskMemFree(temp);
//	}
//}