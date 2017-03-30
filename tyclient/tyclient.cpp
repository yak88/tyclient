// tyclient.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "tyclient.h"

#include <windows.h>
#include "simple_app.h"
#include "simple_handler.h"

#define MAX_LOADSTRING 100

#ifdef _DEBUG  
	#pragma comment(lib, "libcef\\Debug\\libcef")  
    #pragma comment(lib, "libcef\\Debug\\libcef_dll_wrapper")  
#else  
    #pragma comment(lib, "libcef\\Release\\libcef")  
    #pragma comment(lib, "libcef\\Release\\libcef_dll_wrapper")  
#endif  

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

CefRefPtr<SimpleHandler> g_handler; 

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。

	// Enable High-DPI support on Windows 7 or newer.
	CefEnableHighDPISupport();

	// Provide CEF with command-line arguments.
	CefMainArgs main_args(hInstance);	

	// SimpleApp implements application-level callbacks for the browser process.
	// It will create the first browser instance in OnContextInitialized() after
	// CEF has initialized.
	CefRefPtr<SimpleApp> app(new SimpleApp);

	// CEF applications have multiple sub-processes (render, plugin, GPU, etc)
	// that share the same executable. This function checks the command-line and,
	// if this is a sub-process, executes the appropriate logic.
	int exit_code = CefExecuteProcess(main_args, app.get(), NULL);
	if (exit_code >= 0) {
		// The sub-process has completed so return here.
		return exit_code;
	}

	// Specify CEF global settings here.
	CefSettings settings;
	settings.no_sandbox = true;
	//settings.single_process = true;
	//settings.multi_threaded_message_loop = true;
	// Initialize CEF.
	CefInitialize(main_args, settings, app.get(), NULL);
	
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TYCLIENT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TYCLIENT));


	// Run the CEF message loop. This will block until CefQuitMessageLoop() is
	// called.
	CefRunMessageLoop();

	// Shut down CEF.
	CefShutdown();

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// Shut down CEF.
	//CefShutdown();

	return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释:
//
//    仅当希望
//    此代码与添加到 Windows 95 中的“RegisterClassEx”
//    函数之前的 Win32 系统兼容时，才需要此函数及其用法。调用此函数十分重要，
//    这样应用程序就可以获得关联的
//    “格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TYCLIENT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TYCLIENT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
    case WM_CREATE:  
        {
			g_handler=new SimpleHandler(); 
            
			RECT rect;  
            ::GetClientRect(hWnd, &rect);  
			//CefWindowInfo info;  
			//CefBrowserSettings settings; 
			//info.SetAsChild(hWnd, rect); 

			//CefBrowserHost::CreateBrowser(info, g_handler.get(), "http://www.baidu.com", settings, NULL); 
            g_handler->CreateBrowser(hWnd, rect);
            break;  
        }  
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			{			
			if ( g_handler.get() && !g_handler->IsClosing() )  
            {   
                CefRefPtr<CefBrowser> pBrowser=g_handler->GetBrowser();
				if ( pBrowser.get() ){  
                    pBrowser->GetHost()->CloseBrowser(false);	
					pBrowser = NULL;
					return 0;
				}
            }			
			DestroyWindow(hWnd);		
			break;
			}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
        {  
            if ( wParam == SIZE_MINIMIZED   
                || NULL == g_handler  
                || NULL == g_handler->GetBrowserHostWnd() )  
                break;  
            HWND hBrowserWnd=g_handler->GetBrowserHostWnd();  
            RECT rect;  
            ::GetClientRect(hWnd, &rect);  
            ::MoveWindow(hBrowserWnd, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE);  
            break;  
        }	
    case WM_CLOSE:  
        {  			
            if ( g_handler.get() && !g_handler->IsClosing() )  
            {   
                CefRefPtr<CefBrowser> pBrowser=g_handler->GetBrowser();  
				if ( pBrowser.get() )  {
                    pBrowser->GetHost()->CloseBrowser(false);
					pBrowser = NULL;
					return 0;
				}

            }			
			//DestroyWindow(g_handler->GetBrowserHostWnd());
			//g_handler=NULL;
			return DefWindowProc(hWnd, message, wParam, lParam);
        }			
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
