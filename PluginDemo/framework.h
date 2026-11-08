#pragma once

#define WIN32_LEAN_AND_MEAN             // Loại bỏ các nội dung hiếm khi dùng khỏi header Windows
//// Tệp header Windows
//#include <windows.h>

#include <SDKDDKVer.h>

#include <afxwin.h>         // Thành phần lõi và thành phần chuẩn của MFC
#include <afxext.h>         // Phần mở rộng MFC
#include <afxdisp.h>        // Lớp tự động hóa (automation) MFC

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif
