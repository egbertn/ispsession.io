#pragma once
enum SAMESITEPOLICY
{
	Strict = 1,
	Lax = 2,
	None = 3
};
MIDL_INTERFACE("70f2328c-f21d-4072-bfce-571803c9a4c6")
IWriteSessionCookie: public IUnknown
{
	STDMETHOD(put_Item)(BSTR key, BSTR bstrValue)noexcept = 0;
// relative expiration in minutes
	STDMETHOD(put_Expires)(LONG expires)noexcept = 0;
	STDMETHOD(put_Domain)(BSTR bstrDomain)noexcept = 0;
	STDMETHOD(put_Path)(BSTR bstrPath)noexcept = 0;
	STDMETHOD(put_Secure)(VARIANT_BOOL fSecure) noexcept = 0;
	STDMETHOD(put_SameSite)(SAMESITEPOLICY sameSite)noexcept = 0;
};