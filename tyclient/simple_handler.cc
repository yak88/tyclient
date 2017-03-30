// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "simple_handler.h"
#include "simple_context_handler.h"

#include <sstream>
#include <string>

#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "include/cef_runnable.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

namespace {

SimpleHandler* g_instance = NULL;

}  // namespace

SimpleHandler::SimpleHandler()
    : is_closing_(false) {
  DCHECK(!g_instance);
  g_instance = this;
}

SimpleHandler::~SimpleHandler() {
  g_instance = NULL;
}

// static
SimpleHandler* SimpleHandler::GetInstance() {
  return g_instance;
}

void SimpleHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();

  // Add to the list of existing browsers.
  browser_list_.push_back(browser);
  
  if ( ! m_pBrowser.get() ) { 
	base::AutoLock scopLock(lock_); 
	m_pBrowser=browser;  
  }
}

bool SimpleHandler::DoClose(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();

  // Closing the main window requires special handling. See the DoClose()
  // documentation in the CEF header for a detailed destription of this
  // process.
  if (browser_list_.size() == 1) {
    // Set a flag to indicate that the window close should be allowed.
    is_closing_ = true;
  }

  // Allow the close. For windowed browsers this will result in the OS close
  // event being sent.
  return false;
}

void SimpleHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();

  // Remove from the list of existing browsers.
  BrowserList::iterator bit = browser_list_.begin();
  for (; bit != browser_list_.end(); ++bit) {
    if ((*bit)->IsSame(browser)) {
      browser_list_.erase(bit);
      break;
    }
  }


  if (browser_list_.empty()) {
    // All browser windows have closed. Quit the application message loop.
	base::AutoLock lock_scope(lock_);
	
	//CefRefPtr<CefCookieManager> manager = m_pContextHandler->GetCookieManager();
	CefRefPtr<CefCookieManager> manager = CefRequestContext::GetGlobalContext()->GetDefaultCookieManager(NULL);
    if (manager){
		manager->FlushStore(NULL);
    }
	//m_pContextHandler = NULL;
	//CefPostTask(TID_UI, CefCreateClosureTask(base::Bind(base::IgnoreResult(&CefCookieManager::FlushStore), manager,NULL)));
	//CefPostTask(TID_IO, NewCefRunnableMethod((const int)manager.get(), &CefCookieManager::FlushStore, NULL));	
	m_pBrowser = NULL;
	    
	CefQuitMessageLoop();
  }
}

void SimpleHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                ErrorCode errorCode,
                                const CefString& errorText,
                                const CefString& failedUrl) {
  CEF_REQUIRE_UI_THREAD();

  // Don't display an error for downloaded files.
  if (errorCode == ERR_ABORTED)
    return;

  // Display a load error message.
  std::wstringstream ss;
  ss << L"<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"></head>" <<
	  L"<body bgcolor=\"white\">"<<
	  L"<h2 align=\"center\">页面加载失败！</h2> <p><center>页面地址： " << 
	  std::wstring(failedUrl) <<
      L"，错误信息： " << std::wstring(errorText) << 
	  L" (" << errorCode <<
	  L")。<p> <a href=\"" << std::wstring(failedUrl) << 
	  L"\">点击重新加载</a></center></body></html>"<<"\0";
  frame->LoadString(ss.str(), failedUrl);
}

void SimpleHandler::CloseAllBrowsers(bool force_close) {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute on the UI thread.
    CefPostTask(TID_UI,
        base::Bind(&SimpleHandler::CloseAllBrowsers, this, force_close));
    return;
  }

  if (browser_list_.empty())
    return;

  BrowserList::const_iterator it = browser_list_.begin();
  for (; it != browser_list_.end(); ++it)
    (*it)->GetHost()->CloseBrowser(force_close);
}

void SimpleHandler::CreateBrowser( HWND hParentWnd, const RECT& rect )  
{  
    CefWindowInfo info;  
    CefBrowserSettings settings;  
    //static wchar_t* pCharset = L"GB2312";  
    //settings.default_encoding.str = pCharset;  
    //settings.default_encoding.length = wcslen(pCharset);  
    info.SetAsChild(hParentWnd, rect);  

	
	//CefRequestContextSettings rcsettings;
	//static wchar_t* pPath =  L"cookies";
	//rcsettings.cache_path.str = pPath;
	//rcsettings.cache_path.length = wcslen(pPath);
	//m_pContextHandler = new SimpleContextHandler();
	CefRefPtr<CefRequestContext> rc = CefRequestContext::CreateContext(CefRequestContext::GetGlobalContext(), new SimpleContextHandler());

	CefBrowserHost::CreateBrowser(info, this, "http://www.baidu.com", settings, rc);  //http://expc.tengzone.com/express_site_pcend/index
}

void SimpleHandler::OnBeforeContextMenu(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefContextMenuParams> params,
    CefRefPtr<CefMenuModel> model) {
  CEF_REQUIRE_UI_THREAD();

  if (params->GetTypeFlags() == CM_TYPEFLAG_PAGE || params->GetTypeFlags() == CM_TYPEFLAG_FRAME) {

    model->AddItem(MENU_ID_RELOAD, "&Reload");

  }
}

bool SimpleHandler::OnContextMenuCommand(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefContextMenuParams> params,
    int command_id,
    EventFlags event_flags) {
  CEF_REQUIRE_UI_THREAD();

	return false;
}