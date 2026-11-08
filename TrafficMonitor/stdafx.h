
// stdafx.h : tệp include cho các tệp include hệ thống chuẩn,
// hoặc các tệp dùng thường xuyên nhưng ít thay đổi
// Các tệp include riêng của dự án

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Loại bỏ các nội dung hiếm khi dùng khỏi header Windows
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // Một số hàm khởi tạo CString sẽ là explicit

// Tắt việc MFC ẩn một số cảnh báo phổ biến nhưng thường có thể bỏ qua an toàn
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // Thành phần lõi và thành phần chuẩn của MFC
#include <afxext.h>         // Phần mở rộng MFC


#include <afxdisp.h>        // Lớp tự động hóa (automation) MFC



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Hỗ trợ của MFC cho các điều khiển chung của Internet Explorer 4
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Hỗ trợ của MFC cho các điều khiển chung của Windows
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // Hỗ trợ của MFC cho ribbon và thanh điều khiển

#include "resource.h"
#include <string>
using std::string;
using std::wstring;
#include <vector>
using std::vector;
#include <deque>
using std::deque;
#include <list>
using std::list;
#include<iostream>
#include<fstream>
using std::ifstream;
using std::ofstream;
#include <map>
#include <set>
#include<io.h>
#include<algorithm>
#include <cmath>
#include <afxinet.h>    //Dùng để hỗ trợ các lớp liên quan đến mạng
#include <afxwin.h>

#include "language.h"
#include "ForkConfig.h"     //Cấu hình riêng của bản Việt hóa

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


#define MY_WM_NOTIFYICON (WM_USER+1005)
#define WM_TASKBAR_WND_CLOSED (WM_USER+1006)        //Gửi thông điệp này khi cửa sổ thanh tác vụ đóng
#define WM_MONITOR_INFO_UPDATED (WM_USER+1007)        //Thông tin giám sát đã được cập nhật
#define WM_REOPEN_TASKBAR_WND (WM_USER+1008)        //Mở lại cửa sổ thanh tác vụ
#define WM_SETTINGS_APPLIED (WM_USER+1009)          //Đã nhấn nút “Áp dụng” trong Tùy chọn

#define WM_PUBLIC_IP_ACQUIRED (WM_USER+1011)        //Luồng lấy IP công cộng đã có kết quả (wParam: 0 = IPv4, 1 = IPv6)
#define WM_REINIT_CONNECTION (WM_USER+1012)         //Luồng giám sát yêu cầu khởi tạo lại kết nối mạng trên luồng giao diện (wParam: lý do)

#define WM_NEXT_USER_MSG (WM_USER+1013)

//#define CONFIG_PATH _T(".\\config.ini")
//#define CONFIG_PATHA ".\\config.ini"
//#define LOG_PATH _T(".\\error.log")
//#define HISTORY_TRAFFIC_PATH _T(".\\history_traffic.dat")
#define MAX_RESTART_CNT 10      //Số lần khởi tạo lại tối đa

#define MAIN_TIMER 1234         //ID của bộ định thời chính
#define DELAY_TIMER 1235        //ID bộ định thời trễ
#define TASKBAR_TIMER 1236
#define CONNECTION_DETAIL_TIMER 1237
#define MONITOR_TIMER 1238
#define DELETE_NOTIFY_ICON_TIMER 1239   //Bộ định thời xóa biểu tượng vùng thông báo
#define RESTART_TASKBAR_TIMER 1240
#define INIT_CONNECT_TIMER 1241
#define DPI_CHANGE_TIMER 1242

#define MAX_INSERT_TO_TASKBAR_CNT 200     //Số lần tối đa thử nhúng vào thanh tác vụ
#define WARN_INSERT_TO_TASKBAR_CNT 20     //Số lần thử nhúng vào thanh tác vụ thì cảnh báo

#define APP_NAME _T("TrafficMonitor")
#define TASKBAR_WINDOW_NAME _T("TrafficMonitorTaskbarWindow")
#define APP_CLASS_NAME _T("TrafficMonitor_r7XZaS4p") //Tên lớp cửa sổ chính của chương trình
#define VERSION L"1.88"
#define COPYRITE_YEAR L"2017-2026"

#define MAX_NOTIFY_ICON 6       //Số lượng biểu tượng vùng thông báo có thể chọn

//Tên tệp ảnh nền
#define BACKGROUND_IMAGE_S L"\\background.bmp"
#define BACKGROUND_IMAGE_L L"\\background_l.bmp"
#define BACKGROUND_IMAGE_S_PNG L"background.png"
#define BACKGROUND_IMAGE_L_PNG L"background_l.png"
#define BACKGROUND_MASK_S L"\\background_mask.bmp"
#define BACKGROUND_MASK_L L"\\background_mask_l.bmp"

#define MIN_FONT_SIZE 5
#define MAX_FONT_SIZE 72

#define SAFE_DELETE(p) do \
{\
    if(p != nullptr) \
    { \
        delete p; \
        p = nullptr; \
    } \
} while (false)

//Định nghĩa hai màu dùng làm màu trong suốt của thanh tác vụ (dùng khi cần đặt cửa sổ thanh tác vụ không trong suốt)
#define TASKBAR_TRANSPARENT_COLOR1 RGB(52, 28, 41)
#define TASKBAR_TRANSPARENT_COLOR2 RGB(38, 67, 55)

//Nếu cần biên dịch cho Windows XP, hãy bỏ chú thích dòng mã bên dưới
//#define COMPILE_FOR_WINXP

//Có tắt tính năng kiểm tra WebExperience hay không, dùng để kiểm tra trên Windows 11 có “Tiện ích” (Widgets) hay không
//Nếu phần chức năng này gây lỗi biên dịch, hãy bỏ chú thích dòng mã bên dưới
//#define DISABLE_WINDOWS_WEB_EXPERIENCE_DETECTOR
