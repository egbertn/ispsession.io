#include "stdafx.h"
#include "ScriptingContext.h"


STDMETHODIMP ScriptingContext::get_Request(IRequest** ppRequest) noexcept
{
	return this->m_piRequest->QueryInterface(ppRequest);
}
STDMETHODIMP ScriptingContext::get_Response(IResponse** ppResponse) noexcept
{
	return this->m_piREsponse->QueryInterface(ppResponse);
}
STDMETHODIMP ScriptingContext::get_Server(IServer** ppServer) noexcept
{
	return this->m_piServer->QueryInterface(ppServer);
}
STDMETHODIMP ScriptingContext::get_Session(ISessionObject** ) noexcept
{
	return E_NOTIMPL;
}
STDMETHODIMP ScriptingContext::get_Application(IApplicationObject** ) noexcept
{
	return E_NOTIMPL;
}

STDMETHODIMP ScriptingContext::Initialize(IDispatch* iRequest, IDispatch* iSErver, IDispatch* iResponse) noexcept
{
	iRequest->QueryInterface(&this->m_piRequest);
	iSErver->QueryInterface(&this->m_piServer);
	iResponse->QueryInterface(&this->m_piREsponse);
	return S_OK;
}
void ScriptingContext::FinalRelease() 
{
	try {
		m_piRequest.Release();
		m_piServer.Release();
		m_piREsponse.Release();
	}
	catch (...)
	{

	}
}