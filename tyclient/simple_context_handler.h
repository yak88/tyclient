#ifndef CEF_TESTS_CEFSIMPLE_SIMPLE_CONTEXT_HANDLER_H_
#define CEF_TESTS_CEFSIMPLE_SIMPLE_CONTEXT_HANDLER_H_

#include "include/cef_app.h"

class SimpleContextHandler : public CefRequestContextHandler
{
public:
	virtual CefRefPtr<CefCookieManager> GetCookieManager() override
	{ 
		if (NULL!=coke_)
		{
			return coke_;
		}
		coke_ = CefCookieManager::CreateManager("cookies", false, NULL);
		return coke_;
	}

private:
	CefRefPtr<CefCookieManager> coke_;

private:
// Include the default reference counting implementation.
IMPLEMENT_REFCOUNTING(SimpleContextHandler);
}; 

#endif  // CEF_TESTS_CEFSIMPLE_SIMPLE_CONTEXT_HANDLER_H_