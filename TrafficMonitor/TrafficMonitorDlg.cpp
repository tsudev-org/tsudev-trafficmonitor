
// TrafficMonitorDlg.cpp : tệp triển khai
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "TrafficMonitorDlg.h"
#include "afxdialogex.h"
#include "Test.h"
#include "PluginManagerDlg.h"
#include "SetItemOrderDlg.h"
#include "WindowsSettingHelper.h"
#include "PluginInfoDlg.h"
#include "WIC.h"
#include "SupportedRenderEnums.h"
#include "ClassicalTaskbarDlg.h"
#include "Win11TaskbarDlg.h"
#include "WineTaskbarDlg.h"
#include "TaskbarHelper.h"
#include "SkinManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// Hộp thoại CTrafficMonitorDlg

//Khởi tạo thành viên tĩnh
unsigned int CTrafficMonitorDlg::m_WM_TASKBARCREATED{ ::RegisterWindowMessage(_T("TaskbarCreated")) };  //Đăng ký thông điệp khi thanh tác vụ được tạo

CTrafficMonitorDlg::CTrafficMonitorDlg(CWnd* pParent /*=NULL*/)
    : CDialog(IDD_TRAFFICMONITOR_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_desktop_dc = ::GetDC(NULL);
}

CTrafficMonitorDlg::~CTrafficMonitorDlg()
{
    free(m_pIfTable);

    if (m_tBarDlg != nullptr)
    {
        delete m_tBarDlg;
        m_tBarDlg = nullptr;
    }

    ::ReleaseDC(NULL, m_desktop_dc);

    // Destroy notify area icon handles loaded via LoadImage in OnInitDialog
    // LoadImage creates a copy of the icon resource; each must be freed with DestroyIcon
    for (int i = 0; i < MAX_NOTIFY_ICON; i++)
    {
        if (theApp.m_notify_icons[i] != NULL)
        {
            ::DestroyIcon(theApp.m_notify_icons[i]);
            theApp.m_notify_icons[i] = NULL;
        }
    }
}

CTaskBarDlg* CTrafficMonitorDlg::GetTaskbarWindow() const
{
    if (IsTaskbarWndValid())
        return m_tBarDlg;
    else
        return nullptr;
}

CTrafficMonitorDlg* CTrafficMonitorDlg::Instance()
{
    return dynamic_cast<CTrafficMonitorDlg*>(theApp.m_pMainWnd);
}

void CTrafficMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTrafficMonitorDlg, CDialog)
    ON_WM_QUERYDRAGICON()
    ON_WM_TIMER()
    ON_WM_RBUTTONUP()
    ON_WM_LBUTTONDOWN()
    ON_COMMAND(ID_NETWORK_INFO, &CTrafficMonitorDlg::OnNetworkInfo)
    ON_COMMAND(ID_ALWAYS_ON_TOP, &CTrafficMonitorDlg::OnAlwaysOnTop)
    ON_WM_INITMENUPOPUP()
    ON_COMMAND(ID_TRANSPARENCY_100, &CTrafficMonitorDlg::OnTransparency100)
    ON_COMMAND(ID_TRANSPARENCY_80, &CTrafficMonitorDlg::OnTransparency80)
    ON_COMMAND(ID_TRANSPARENCY_60, &CTrafficMonitorDlg::OnTransparency60)
    ON_COMMAND(ID_TRANSPARENCY_40, &CTrafficMonitorDlg::OnTransparency40)
    ON_WM_CLOSE()
    ON_WM_INITMENU()
    ON_COMMAND(ID_LOCK_WINDOW_POS, &CTrafficMonitorDlg::OnLockWindowPos)
    ON_WM_MOVE()
    ON_MESSAGE(MY_WM_NOTIFYICON, &CTrafficMonitorDlg::OnNotifyIcon)
    ON_COMMAND(ID_SHOW_NOTIFY_ICON, &CTrafficMonitorDlg::OnShowNotifyIcon)
    ON_WM_DESTROY()
    ON_COMMAND(ID_SHOW_CPU_MEMORY, &CTrafficMonitorDlg::OnShowCpuMemory)
    ON_COMMAND(ID_MOUSE_PENETRATE, &CTrafficMonitorDlg::OnMousePenetrate)
    ON_COMMAND(ID_SHOW_TASK_BAR_WND, &CTrafficMonitorDlg::OnShowTaskBarWnd)
    ON_COMMAND(ID_APP_ABOUT, &CTrafficMonitorDlg::OnAppAbout)
    ON_COMMAND(ID_SHOW_CPU_MEMORY2, &CTrafficMonitorDlg::OnShowCpuMemory2)
    ON_COMMAND(ID_SHOW_MAIN_WND, &CTrafficMonitorDlg::OnShowMainWnd)
    ON_COMMAND(ID_CHANGE_SKIN, &CTrafficMonitorDlg::OnChangeSkin)
    ON_REGISTERED_MESSAGE(m_WM_TASKBARCREATED, &CTrafficMonitorDlg::OnTaskBarCreated)
    ON_COMMAND(ID_TRAFFIC_HISTORY, &CTrafficMonitorDlg::OnTrafficHistory)
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDBLCLK()
    ON_COMMAND(ID_OPTIONS, &CTrafficMonitorDlg::OnOptions)
    ON_COMMAND(ID_OPTIONS2, &CTrafficMonitorDlg::OnOptions2)
    ON_MESSAGE(WM_EXITMENULOOP, &CTrafficMonitorDlg::OnExitmenuloop)
    ON_COMMAND(ID_CHANGE_NOTIFY_ICON, &CTrafficMonitorDlg::OnChangeNotifyIcon)
    ON_COMMAND(ID_ALOW_OUT_OF_BORDER, &CTrafficMonitorDlg::OnAlowOutOfBorder)
    ON_COMMAND(ID_CHECK_UPDATE, &CTrafficMonitorDlg::OnCheckUpdate)
    ON_MESSAGE(WM_TASKBAR_MENU_POPED_UP, &CTrafficMonitorDlg::OnTaskbarMenuPopedUp)
    ON_COMMAND(ID_SHOW_NET_SPEED, &CTrafficMonitorDlg::OnShowNetSpeed)
    ON_WM_QUERYENDSESSION()
    ON_WM_PAINT()
    ON_MESSAGE(WM_DPICHANGED, &CTrafficMonitorDlg::OnDpichanged)
    ON_MESSAGE(WM_TASKBAR_WND_CLOSED, &CTrafficMonitorDlg::OnTaskbarWndClosed)
    ON_MESSAGE(WM_MONITOR_INFO_UPDATED, &CTrafficMonitorDlg::OnMonitorInfoUpdated)
    ON_MESSAGE(WM_DISPLAYCHANGE, &CTrafficMonitorDlg::OnDisplaychange)
    ON_WM_EXITSIZEMOVE()
    ON_COMMAND(ID_PLUGIN_MANAGE, &CTrafficMonitorDlg::OnPluginManage)
    ON_MESSAGE(WM_REOPEN_TASKBAR_WND, &CTrafficMonitorDlg::OnReopenTaksbarWnd)
    ON_COMMAND(ID_OPEN_TASK_MANAGER, &CTrafficMonitorDlg::OnOpenTaskManager)
    ON_MESSAGE(WM_SETTINGS_APPLIED, &CTrafficMonitorDlg::OnSettingsApplied)
    ON_MESSAGE(WM_REINIT_CONNECTION, &CTrafficMonitorDlg::OnReinitConnection)
    ON_COMMAND(ID_DISPLAY_SETTINGS, &CTrafficMonitorDlg::OnDisplaySettings)
    ON_WM_LBUTTONUP()
    ON_COMMAND(ID_REFRESH_CONNECTION_LIST, &CTrafficMonitorDlg::OnRefreshConnectionList)
    ON_MESSAGE(WM_TABLET_QUERYSYSTEMGESTURESTATUS, &CTrafficMonitorDlg::OnTabletQuerysystemgesturestatus)
    ON_COMMAND(ID_PLUGIN_OPTIONS, &CTrafficMonitorDlg::OnPluginOptions)
    ON_COMMAND(ID_PLUGIN_DETAIL, &CTrafficMonitorDlg::OnPluginDetail)
    ON_COMMAND(ID_PLUGIN_OPTIONS_TASKBAR, &CTrafficMonitorDlg::OnPluginOptionsTaksbar)
    ON_COMMAND(ID_PLUGIN_DETAIL_TASKBAR, &CTrafficMonitorDlg::OnPluginDetailTaksbar)
    ON_WM_POWERBROADCAST()
    ON_WM_DWMCOLORIZATIONCOLORCHANGED()
END_MESSAGE_MAP()


CString CTrafficMonitorDlg::GetMouseTipsInfo()
{
    CString tip_info;
    CString temp;
    temp.Format(_T("%s: %s\r\n (%s: %s/%s: %s)"), CCommon::LoadText(IDS_TRAFFIC_USED_TODAY),
        CCommon::KBytesToString((theApp.m_today_up_traffic + theApp.m_today_down_traffic) / 1024u),
        CCommon::LoadText(IDS_UPLOAD), CCommon::KBytesToString(theApp.m_today_up_traffic / 1024u),
        CCommon::LoadText(IDS_DOWNLOAD), CCommon::KBytesToString(theApp.m_today_down_traffic / 1024u)
    );
    tip_info += temp;
    const CSkinFile::Layout& skin_layout{ theApp.m_cfg_data.m_show_more_info ? m_skin.GetLayoutInfo().layout_l : m_skin.GetLayoutInfo().layout_s }; //Bố cục skin hiện tại
    if (!skin_layout.GetItem(TDI_UP).show)      //Nếu cửa sổ chính không hiển thị tốc độ tải lên thì hiển thị tốc độ tải lên trong thông tin chú giải
    {
        temp.Format(_T("\r\n%s: %s/s"), CCommon::LoadText(IDS_UPLOAD),
            CCommon::DataSizeToString(theApp.m_out_speed, theApp.m_main_wnd_data));
        tip_info += temp;
    }
    if (!skin_layout.GetItem(TDI_DOWN).show)
    {
        temp.Format(_T("\r\n%s: %s/s"), CCommon::LoadText(IDS_DOWNLOAD),
            CCommon::DataSizeToString(theApp.m_in_speed, theApp.m_main_wnd_data));
        tip_info += temp;
    }
    if (!skin_layout.GetItem(TDI_CPU).show)
    {
        temp.Format(_T("\r\n%s: %d %%"), CCommon::LoadText(IDS_CPU_USAGE), theApp.m_cpu_usage);
        tip_info += temp;
    }
    if (!skin_layout.GetItem(TDI_MEMORY).show)
    {
        temp.Format(_T("\r\n%s: %s/%s (%d %%)"), CCommon::LoadText(IDS_MEMORY_USAGE),
            CCommon::KBytesToString(theApp.m_used_memory),
            CCommon::KBytesToString(theApp.m_total_memory), theApp.m_memory_usage);
        tip_info += temp;
    }
    else
    {
        temp.Format(_T("\r\n%s: %s/%s"), CCommon::LoadText(IDS_MEMORY_USAGE),
            CCommon::KBytesToString(theApp.m_used_memory),
            CCommon::KBytesToString(theApp.m_total_memory));
        tip_info += temp;
    }
    if (!skin_layout.GetItem(TDI_CPU_FREQ).show && theApp.m_cpu_freq >= 0)
    {
        temp.Format(_T("\r\n%s: %s"), CCommon::LoadText(IDS_CPU_FREQ), CCommon::FreqToString(theApp.m_cpu_freq, theApp.m_main_wnd_data));
        tip_info += temp;
    }
    if (!skin_layout.GetItem(TDI_GPU_USAGE).show && theApp.m_gpu_usage >= 0)
    {
        temp.Format(_T("\r\n%s: %d %%"), CCommon::LoadText(IDS_GPU_USAGE), theApp.m_gpu_usage);
        tip_info += temp;
    }
#ifndef WITHOUT_TEMPERATURE
    if (IsTemperatureNeeded())
    {
        if (theApp.m_general_data.IsHardwareEnable(HI_CPU) && !skin_layout.GetItem(TDI_CPU_TEMP).show && theApp.m_cpu_temperature > 0)
        {
            temp.Format(_T("\r\n%s: %s"), CCommon::LoadText(IDS_CPU_TEMPERATURE), CCommon::TemperatureToString(theApp.m_cpu_temperature, theApp.m_main_wnd_data));
            tip_info += temp;
        }
        if (theApp.m_general_data.IsHardwareEnable(HI_GPU) && !skin_layout.GetItem(TDI_GPU_TEMP).show && theApp.m_gpu_temperature > 0)
        {
            temp.Format(_T("\r\n%s: %s"), CCommon::LoadText(IDS_GPU_TEMPERATURE), CCommon::TemperatureToString(theApp.m_gpu_temperature, theApp.m_main_wnd_data));
            tip_info += temp;
        }
        if (theApp.m_general_data.IsHardwareEnable(HI_HDD) && !skin_layout.GetItem(TDI_HDD_TEMP).show && theApp.m_hdd_temperature > 0)
        {
            temp.Format(_T("\r\n%s: %s"), CCommon::LoadText(IDS_HDD_TEMPERATURE), CCommon::TemperatureToString(theApp.m_hdd_temperature, theApp.m_main_wnd_data));
            tip_info += temp;
        }
        if (theApp.m_general_data.IsHardwareEnable(HI_MBD) && !skin_layout.GetItem(TDI_MAIN_BOARD_TEMP).show && theApp.m_main_board_temperature > 0)
        {
            temp.Format(_T("\r\n%s: %s"), CCommon::LoadText(IDS_MAINBOARD_TEMPERATURE), CCommon::TemperatureToString(theApp.m_main_board_temperature, theApp.m_main_wnd_data));
            tip_info += temp;
        }
    }
#endif
    if (!skin_layout.GetItem(TDI_HDD_USAGE).show && theApp.m_hdd_usage >= 0)
    {
        temp.Format(_T("\r\n%s: %d %%"), CCommon::LoadText(IDS_HDD_USAGE), theApp.m_hdd_usage);
        tip_info += temp;
}

    //Thêm chú giải chuột cho mục plugin
    tip_info += theApp.GetPlauginTooltipInfo().c_str();

    //if (IsTaskbarWndValid())
    //{
    //    tip_info += L"\r\n";
    //    tip_info += m_tBarDlg->GetTaskbarDebugString().c_str();
    //}

    return tip_info;
}

void CTrafficMonitorDlg::SetTransparency()
{
    SetTransparency(theApp.m_cfg_data.m_transparency);
}

void CTrafficMonitorDlg::SetTransparency(int transparency)
{
    SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    if (m_skin.IsPNG())
    {
        m_skin.SetAlpha(transparency * 255 / 100);
    }
    else
    {
        SetLayeredWindowAttributes(0, static_cast<BYTE>(transparency * 255 / 100), LWA_ALPHA);  //Phạm vi giá trị độ trong suốt là 0~255
    }
}

void CTrafficMonitorDlg::SetAlwaysOnTop()
{
    //if (!m_is_foreground_fullscreen || (m_is_foreground_fullscreen && !theApp.m_main_wnd_data.hide_main_wnd_when_fullscreen))
    //{
    //  if (m_always_on_top)
    //      SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);         //Ghim trên cùng
    //  else
    //      SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);       //Bỏ ghim trên cùng
    //}
    if (theApp.m_cfg_data.m_hide_main_window)
        return;
    else if (theApp.m_main_wnd_data.hide_main_wnd_when_fullscreen && m_is_foreground_fullscreen)        //Khi đã bật ẩn cửa sổ nổi khi có chương trình toàn màn hình và đang có chương trình chạy toàn màn hình thì không ghim trên cùng
        return;

    if (theApp.m_main_wnd_data.m_always_on_top)
        SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);         //Đặt luôn trên cùng
    else
        SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);       //Bỏ ghim trên cùng

}

void CTrafficMonitorDlg::SetMousePenetrate()
{
    if (theApp.m_main_wnd_data.m_mouse_penetrate)
    {
        SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT);     //Thiết lập cho chuột xuyên qua
    }
    else
    {
        SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) & (~WS_EX_TRANSPARENT));      //Hủy cho chuột xuyên qua
    }
}

POINT CTrafficMonitorDlg::CalculateWindowMoveOffset(CRect rect, bool screen_changed)
{
    POINT mov{};    // Độ lệch cần thiết
    if (m_screen_rects.size() != 0)
    {
        // Đảm bảo cửa sổ nằm trọn trong một màn hình và hiển thị được; xét khoảng cách di chuyển và dịch theo hướng cần di chuyển ít hơn
        LONG mov_xy = 0;          // Ghi lại khoảng cách di chuyển
        int i = 0;
        for (auto& a : m_screen_rects)
        {
            LONG x = 0, y = 0;
            if (rect.left < a.left)                 // Cần di chuyển sang phải
                x = a.left - rect.left;
            if (rect.top < a.top)                   // Cần di chuyển xuống dưới
                y = a.top - rect.top;

            //Tránh khi kết nối màn hình mới, số phần tử của m_last_screen_rects nhỏ hơn m_screen_rects dẫn đến truy cập chỉ số quá lớn
            if (i >= static_cast<int>(m_last_screen_rects.size()))
            {
                break;
            }
            CRect last_screen_rect = m_last_screen_rects[i];
            if (screen_changed && a != last_screen_rect)
            {
                float proportion_width = (float)rect.left / (last_screen_rect.right - rect.Width());
                float proportion_height = (float)rect.top / (last_screen_rect.bottom - rect.Height());
                x = static_cast<LONG>((a.right - rect.Width()) * proportion_width - rect.left);
                y = static_cast<LONG>((a.bottom - rect.Height()) * proportion_height - rect.top);
            }
            else
            {
                if (rect.right > a.right)          // Cần di chuyển sang trái
                    x = a.right - rect.right;
                if (rect.bottom > a.bottom)        // Cần di chuyển lên trên
                    y = a.bottom - rect.bottom;
            }
            if (x == 0 && y == 0)           // Cửa sổ đã nằm trong một màn hình
            {
                mov.x = 0;
                mov.y = 0;
                break;
            }
            else if (abs(x) + abs(y) < mov_xy || mov_xy == 0)
            {
                mov.x = x;
                mov.y = y;
                mov_xy = abs(x) + abs(y);
            }
            i++;
        }
    }
    return mov;
}

void CTrafficMonitorDlg::CheckWindowPos(bool screen_changed)
{
    if (!theApp.m_main_wnd_data.m_alow_out_of_border)
    {
        CRect rect;
        GetWindowRect(rect);
        MoveWindow(rect + CalculateWindowMoveOffset(rect, screen_changed));
    }
}

void CTrafficMonitorDlg::GetScreenSize()
{
    m_screen_size.cx = GetSystemMetrics(SM_CXSCREEN);
    m_screen_size.cy = GetSystemMetrics(SM_CYSCREEN);

    //::SystemParametersInfo(SPI_GETWORKAREA, 0, &m_screen_rect, 0);   // Lấy kích thước vùng làm việc

    //Lấy kích thước vùng làm việc của tất cả màn hình
    m_last_screen_rects = m_screen_rects;
    m_screen_rects.clear();
    Monitors monitors;
    for (auto& a : monitors.monitorinfos)
    {
        m_screen_rects.push_back(a.rcWork);
    }
}


void CTrafficMonitorDlg::AutoSelect()
{
    CSingleLock lock(&m_connections_lock, TRUE);
    unsigned __int64 max_in_out_bytes{};
    unsigned __int64 in_out_bytes;
    //m_connection_selected = m_connections[0].index;
    m_connection_selected = 0;
    //Khi tự động chọn kết nối, tìm kết nối có tổng số byte đã gửi và đã nhận lớn nhất rồi đặt làm kết nối đang xem.
    //Ưu tiên kết nối có cổng mặc định (default gateway): card ảo nội bộ như Hyper-V/WSL, VirtualBox
    //thường không có cổng mặc định nhưng có thể có nhiều byte hơn card ra Internet thật (Wi-Fi/LAN).
    //Nếu không kết nối nào có cổng mặc định thì chọn như cũ trong mọi kết nối đang hoạt động.
    auto hasDefaultGateway = [](const wstring& gateway) {
        return !gateway.empty() && gateway != L"0.0.0.0" && gateway != L"-.-.-.-";
    };
    for (int pass = 0; pass < 2; pass++)
    {
        bool found{ false };
        for (int i{}; i < static_cast<int>(m_connections.size()); i++)
        {
            auto table = GetConnectIfTable(i);
            if (table.dwOperStatus != IF_OPER_STATUS_OPERATIONAL)     //Chỉ chọn các kết nối có trạng thái mạng bình thường
                continue;
            if (pass == 0 && !hasDefaultGateway(GetConnection(i).default_gateway))
                continue;
            unsigned __int64 in_bytes{}, out_bytes{};
            CAdapterCommon::GetInterfaceOctets(table, in_bytes, out_bytes);
            in_out_bytes = in_bytes + out_bytes;
            if (!found || in_out_bytes > max_in_out_bytes)
            {
                max_in_out_bytes = in_out_bytes;
                m_connection_selected = i;
                found = true;
            }
        }
        if (found)
            break;
    }
    theApp.m_cfg_data.m_connection_name = GetConnection(m_connection_selected).description_2;
    m_connection_change_flag = true;
}

void CTrafficMonitorDlg::IniConnection()
{
    //Khóa để luồng giám sát không đọc m_pIfTable/m_connections trong lúc chúng được cấp phát lại
    CSingleLock lock(&m_connections_lock, TRUE);

    //Cấp phát vùng nhớ với kích thước cần thiết cho m_pIfTable.
    //Số card mạng có thể thay đổi giữa hai lần gọi GetIfTable nên thử lại vài lần; nếu vẫn thất bại thì dùng bảng rỗng
    //(trước đây bảng chưa được điền vẫn được dùng, dwNumEntries là giá trị rác dẫn tới đọc ngoài vùng nhớ).
    DWORD required_size = sizeof(MIB_IFTABLE);
    DWORD rtn = ERROR_INSUFFICIENT_BUFFER;
    for (int i = 0; i < 5 && rtn == ERROR_INSUFFICIENT_BUFFER; i++)
    {
        MIB_IFTABLE* p_new_table = (MIB_IFTABLE*)malloc(required_size);
        if (p_new_table == nullptr)
            break;
        free(m_pIfTable);
        m_pIfTable = p_new_table;
        m_dwSize = required_size;
        DWORD size = required_size;
        rtn = GetIfTable(m_pIfTable, &size, FALSE);
        if (rtn == ERROR_INSUFFICIENT_BUFFER)
            required_size = size + sizeof(MIB_IFROW) * 4;     //Chừa thêm chỗ cho card mạng mới xuất hiện
    }
    if (rtn != NO_ERROR || m_pIfTable == nullptr)
    {
        if (m_pIfTable == nullptr)
        {
            m_pIfTable = (MIB_IFTABLE*)malloc(sizeof(MIB_IFTABLE));
            m_dwSize = sizeof(MIB_IFTABLE);
        }
        if (m_pIfTable != nullptr)
            m_pIfTable->dwNumEntries = 0;
    }

    //Lấy tất cả kết nối hiện tại và lưu vào container m_connections
    if (!theApp.m_general_data.show_all_interface)
    {
        m_connections.clear();
        vector<NetWorkConection> connections;
        CAdapterCommon::GetAdapterInfo(connections);
        for (const auto& item : connections)
        {
            if (!theApp.m_general_data.connections_hide.Contains(CCommon::StrToUnicode(item.description.c_str())))
                m_connections.push_back(item);
        }
        CAdapterCommon::GetIfTableInfo(m_connections, m_pIfTable);
    }
    else
    {
        CAdapterCommon::GetAllIfTableInfo(m_connections, m_pIfTable);
    }

    //Nếu đã chọn “Chọn tất cả” khi bật “Hiển thị tất cả kết nối mạng” thì đổi thành “Tự động chọn”
    if (theApp.m_general_data.show_all_interface && theApp.m_cfg_data.m_select_all)
    {
        theApp.m_cfg_data.m_select_all = false;
        theApp.m_cfg_data.m_auto_select = true;
    }

    //Ghi nhật ký gỡ lỗi
    if (theApp.m_debug_log)
    {
        CString log_str;
        log_str += _T("Đang khởi tạo kết nối mạng...\n");
        log_str += _T("Danh sách kết nối:\n");
        for (size_t i{}; i < m_connections.size(); i++)
        {
            log_str += m_connections[i].description.c_str();
            log_str += _T(", ");
            log_str += CCommon::IntToString(m_connections[i].index);
            log_str += _T("\n");
        }
        log_str += _T("IfTable:\n");
        for (size_t i{}; i < m_pIfTable->dwNumEntries; i++)
        {
            log_str += CCommon::IntToString(i);
            log_str += _T(" ");
            log_str += CAdapterCommon::GetIfRowDescription(m_pIfTable->table[i]).c_str();
            log_str += _T("\n");
        }
        CCommon::WriteLog(log_str, (theApp.m_config_dir + L".\\connections.log").c_str());
    }

    //if (m_connection_selected < 0 || m_connection_selected >= m_connections.size() || m_auto_select)
    //  AutoSelect();
    //Chọn kết nối mạng
    if (theApp.m_cfg_data.m_auto_select)    //Tự động chọn
    {
        if (m_restart_cnt != -1)    //Khi m_restart_cnt khác -1, tức không phải lần khởi tạo đầu tiên, cần trễ 5 giây rồi mới khởi tạo lại kết nối
        {
            KillTimer(DELAY_TIMER);
            SetTimer(DELAY_TIMER, 5000, NULL);
        }
        else
        {
            AutoSelect();
        }
    }
    else        //Tìm kết nối có tên mạng giống lần chọn trước
    {
        m_connection_selected = 0;
        for (int i{}; i < static_cast<int>(m_connections.size()); i++)
        {
            if (m_connections[i].description_2 == m_connection_name_preferd)
                m_connection_selected = i;
        }
    }
    if (m_connection_selected < 0 || m_connection_selected >= static_cast<int>(m_connections.size()))
        m_connection_selected = 0;
    theApp.m_cfg_data.m_connection_name = GetConnection(m_connection_selected).description_2;

    //Thêm mục tương ứng vào menu dựa trên các kết nối đã lấy được
    IniConnectionMenu(theApp.m_main_menu.GetSubMenu(0)->GetSubMenu(0));      //Thêm mục vào menu con “Chọn kết nối mạng”
    IniConnectionMenu(theApp.m_main_menu_plugin.GetSubMenu(0)->GetSubMenu(0));      //Thêm mục vào menu con “Chọn kết nối mạng”

    IniTaskBarConnectionMenu();     //Khởi tạo menu con “Chọn kết nối mạng” trong cửa sổ thanh tác vụ

    m_restart_cnt++;    //Ghi lại số lần khởi tạo
    m_connection_change_flag = true;
}

MIB_IFROW CTrafficMonitorDlg::GetConnectIfTable(int connection_index)
{
    if (connection_index >= 0 && connection_index < static_cast<int>(m_connections.size()))
    {
        int index = m_connections[connection_index].index;
        if (m_pIfTable != nullptr && index >= 0 && static_cast<DWORD>(index) < m_pIfTable->dwNumEntries)
            return m_pIfTable->table[index];
    }
    return MIB_IFROW();
}

NetWorkConection CTrafficMonitorDlg::GetConnection(int connection_index)
{
    if (connection_index >= 0 && connection_index < static_cast<int>(m_connections.size()))
        return m_connections[connection_index];
    else
        return NetWorkConection();
}

void CTrafficMonitorDlg::IniConnectionMenu(CMenu* pMenu)
{
    ASSERT(pMenu != nullptr);
    if (pMenu != nullptr)
    {
        //Xóa trước tất cả mục menu phía sau ID_SELECT_ALL_CONNECTION
        int start_pos = CCommon::GetMenuItemPosition(pMenu, ID_SELECT_ALL_CONNECTION) + 1;
        while (pMenu->GetMenuItemCount() > start_pos)
        {
            pMenu->DeleteMenu(start_pos, MF_BYPOSITION);
        }

        CString connection_descr;
        for (size_t i{}; i < m_connections.size(); i++)
        {
            connection_descr = CCommon::StrToUnicode(m_connections[i].description.c_str()).c_str();
            pMenu->AppendMenu(MF_STRING | MF_ENABLED, ID_SELECT_ALL_CONNECTION + i + 1, connection_descr);
        }

        //Thêm lệnh “Làm mới danh sách mạng”
        pMenu->AppendMenu(MF_SEPARATOR);
        pMenu->AppendMenu(MF_STRING | MF_ENABLED, ID_REFRESH_CONNECTION_LIST, CCommon::LoadText(IDS_REFRESH_CONNECTION_LIST));
    }
}

void CTrafficMonitorDlg::IniTaskBarConnectionMenu()
{
    //Thêm mục vào menu con “Chọn kết nối mạng”
    IniConnectionMenu(theApp.m_taskbar_menu.GetSubMenu(0)->GetSubMenu(0));
    IniConnectionMenu(theApp.m_taskbar_menu_plugin.GetSubMenu(0)->GetSubMenu(0));
}

void CTrafficMonitorDlg::SetConnectionMenuState(CMenu* pMenu)
{
    if (theApp.m_cfg_data.m_select_all)
        pMenu->CheckMenuRadioItem(0, static_cast<UINT>(m_connections.size() + 1), 1, MF_BYPOSITION | MF_CHECKED);
    else if (theApp.m_cfg_data.m_auto_select)       //Khi m_auto_select là true thì tự động chọn, chọn mục thứ 1 của menu
        pMenu->CheckMenuRadioItem(0, static_cast<UINT>(m_connections.size() + 1), 0, MF_BYPOSITION | MF_CHECKED);
    else        //Khi m_auto_select là false thì không tự động chọn, chọn mục tương ứng theo giá trị m_connection_selected
        pMenu->CheckMenuRadioItem(0, static_cast<UINT>(m_connections.size() + 1), m_connection_selected + 2, MF_BYPOSITION | MF_CHECKED);

    //Khi không đặt là “Chọn tất cả” thì đặt mục đang chọn làm mục menu mặc định (hiển thị in đậm)
    if (!theApp.m_cfg_data.m_select_all)
        pMenu->SetDefaultItem(m_connection_selected + 2, TRUE);
    else
        pMenu->SetDefaultItem(-1, TRUE);
}

void CTrafficMonitorDlg::CloseTaskBarWnd()
{
    if (m_tBarDlg != nullptr)
    {
        if (IsTaskbarWndValid())
            m_tBarDlg->OnCancel();
        delete m_tBarDlg;
        m_tBarDlg = nullptr;
        theApp.m_taskbar_data.update_layered_window_error_code = 0;
    }
}

void CTrafficMonitorDlg::OpenTaskBarWnd()
{
    // Buộc khởi tạo giá trị theApp.m_is_windows11_taskbar
    theApp.CheckWindows11Taskbar();
    if (theApp.m_win_version.IsWine())
        m_tBarDlg = new CWineTaskbarDlg();
    else if (theApp.IsWindows11Taskbar())
        m_tBarDlg = new CWin11TaskbarDlg();
    else
        m_tBarDlg = new CClassicalTaskbarDlg();

    CSupportedRenderEnums supported_render_enums{};
    CTaskBarDlg::DisableRenderFeatureIfNecessary(supported_render_enums);
    auto render_type = supported_render_enums.GetAutoFitEnum();
    // WS_EX_LAYERED và WS_EX_NOREDIRECTIONBITMAP có thể dùng cùng nhau, xem mã mẫu của Microsoft
    // https://github.com/microsoft/Windows-classic-samples/blob/7cbd99ac1d2b4a0beffbaba29ea63d024ceff700/Samples/DynamicDPI/cpp/SampleDesktopWindow.cpp#L179
    // Nhưng WS_EX_NOREDIRECTIONBITMAP dường như khiến UpdateLayeredWindowIndirect thất bại
    switch (render_type)
    {
        using namespace DrawCommonHelper;
    case RenderType::D2D1_WITH_DCOMPOSITION:
        m_tBarDlg->Create(IDD_TASK_BAR_DIALOG_NOREDIRECTIONBITMAP, this);
        break;
    // Các giá trị khác, bao gồm RenderType::D2D1
    default:
        m_tBarDlg->Create(IDD_TASK_BAR_DIALOG, this);
        break;
    }
    m_tBarDlg->ShowWindow(SW_SHOW);
    //m_tBarDlg->ShowInfo();
    //IniTaskBarConnectionMenu();
}

void CTrafficMonitorDlg::AddNotifyIcon()
{
    if (theApp.m_cfg_data.m_show_task_bar_wnd)
        CloseTaskBarWnd();
    //Thêm biểu tượng vùng thông báo
    ::Shell_NotifyIcon(NIM_ADD, &m_ntIcon);
    if (theApp.m_cfg_data.m_show_task_bar_wnd)
        OpenTaskBarWnd();
}

void CTrafficMonitorDlg::DeleteNotifyIcon()
{
    if (theApp.m_cfg_data.m_show_task_bar_wnd)
        CloseTaskBarWnd();
    //Xóa biểu tượng vùng thông báo
    ::Shell_NotifyIcon(NIM_DELETE, &m_ntIcon);
    if (theApp.m_cfg_data.m_show_task_bar_wnd)
        OpenTaskBarWnd();
}

void CTrafficMonitorDlg::ShowNotifyTip(const wchar_t* title, const wchar_t* message)
{
    //Muốn hiện thông báo ở vùng thông báo thì phải hiển thị biểu tượng vùng thông báo trước
    if (!theApp.m_general_data.show_notify_icon)
    {
        //Thêm biểu tượng vùng thông báo
        AddNotifyIcon();
    }
    //Hiển thị thông báo
    m_ntIcon.uFlags |= NIF_INFO;
    //wcscpy_s(m_ntIcon.szInfo, message ? message : _T(""));
    //wcscpy_s(m_ntIcon.szInfoTitle, title ? title : _T(""));
    CCommon::WStringCopy(m_ntIcon.szInfo, 256, message);
    CCommon::WStringCopy(m_ntIcon.szInfoTitle, 64, title);
    ::Shell_NotifyIcon(NIM_MODIFY, &m_ntIcon);
    m_ntIcon.uFlags &= ~NIF_INFO;

    //Nếu không hiển thị biểu tượng vùng thông báo thì xóa biểu tượng sau một khoảng thời gian kể từ khi hiện thông báo
    if (!theApp.m_general_data.show_notify_icon)
    {
        //Xóa biểu tượng vùng thông báo sau một khoảng thời gian trễ
        KillTimer(DELETE_NOTIFY_ICON_TIMER);
        SetTimer(DELETE_NOTIFY_ICON_TIMER, 8000, NULL);
    }
}

void CTrafficMonitorDlg::UpdateNotifyIconTip()
{
    CString strTip;         //Chú giải hiển thị khi trỏ chuột vào biểu tượng
#ifdef _DEBUG
    strTip = CCommon::LoadText(IDS_TRAFFICMONITOR, _T(" (Debug)"));
#else
    strTip = CCommon::LoadText(IDS_TRAFFICMONITOR);
#endif

    CString in_speed = CCommon::DataSizeToString(theApp.m_in_speed);
    CString out_speed = CCommon::DataSizeToString(theApp.m_out_speed);

    strTip += CCommon::StringFormat(_T("\r\n<%1%>: <%2%>/s"), { CCommon::LoadText(IDS_UPLOAD), out_speed });
    strTip += CCommon::StringFormat(_T("\r\n<%1%>: <%2%>/s"), { CCommon::LoadText(IDS_DOWNLOAD), in_speed });
    strTip += CCommon::StringFormat(_T("\r\nCPU: <%1%> %"), { theApp.m_cpu_usage });
    strTip += CCommon::StringFormat(_T("\r\n<%1%>: <%2%> %"), { CCommon::LoadText(IDS_MEMORY), theApp.m_memory_usage });
    if (IsTemperatureNeeded())
    {
        if (theApp.m_general_data.IsHardwareEnable(HI_GPU) && theApp.m_gpu_usage >= 0)
            strTip += CCommon::StringFormat(_T("\r\n<%1%>: <%2%> %"), { CCommon::LoadText(IDS_GPU_USAGE), theApp.m_gpu_usage });
        if (theApp.m_general_data.IsHardwareEnable(HI_CPU) && theApp.m_cpu_temperature > 0)
            strTip += CCommon::StringFormat(_T("\r\n<%1%>: <%2%> °C"), { CCommon::LoadText(IDS_CPU_TEMPERATURE), static_cast<int>(theApp.m_cpu_temperature) });
        if (theApp.m_general_data.IsHardwareEnable(HI_GPU) && theApp.m_gpu_temperature > 0)
            strTip += CCommon::StringFormat(_T("\r\n<%1%>: <%2%> °C"), { CCommon::LoadText(IDS_GPU_TEMPERATURE), static_cast<int>(theApp.m_gpu_temperature) });
        if (theApp.m_general_data.IsHardwareEnable(HI_HDD) && theApp.m_hdd_temperature > 0)
            strTip += CCommon::StringFormat(_T("\r\n<%1%>: <%2%> °C"), { CCommon::LoadText(IDS_HDD_TEMPERATURE), static_cast<int>(theApp.m_hdd_temperature) });
        if (theApp.m_general_data.IsHardwareEnable(HI_MBD) && theApp.m_main_board_temperature > 0)
            strTip += CCommon::StringFormat(_T("\r\n<%1%>: <%2%> °C"), { CCommon::LoadText(IDS_MAINBOARD_TEMPERATURE), static_cast<int>(theApp.m_main_board_temperature) });
        if (theApp.m_general_data.IsHardwareEnable(HI_HDD) && theApp.m_hdd_usage >= 0)
            strTip += CCommon::StringFormat(_T("\r\n<%1%>: <%2%> %"), { CCommon::LoadText(IDS_HDD_USAGE), theApp.m_hdd_usage });
    }

    CCommon::WStringCopy(m_ntIcon.szTip, 128, strTip);
    ::Shell_NotifyIcon(NIM_MODIFY, &m_ntIcon);

}

void CTrafficMonitorDlg::SaveHistoryTraffic()
{
    // Dùng lưu tăng dần, chỉ cập nhật dòng đầu và bản ghi hôm nay, giảm thao tác I/O
    m_history_traffic.SaveTodayOnly();
}

void CTrafficMonitorDlg::SaveHistoryTrafficFull()
{
    // Lưu đầy đủ, dùng khi chương trình thoát để đảm bảo mọi dữ liệu đều được lưu
    m_history_traffic.Save();
}

void CTrafficMonitorDlg::LoadHistoryTraffic()
{
    m_history_traffic.Load();
    CHistoryTrafficFile backup_file(theApp.m_history_traffic_path + L".bak");
    backup_file.LoadSize();     //Đọc số bản ghi lưu lượng trong tệp sao lưu
    
    // Nếu số bản ghi lưu lượng trong tệp sao lưu lớn hơn số hiện tại thì thử khôi phục từ tệp sao lưu
    if (backup_file.Size() > m_history_traffic.Size())
    {
        size_t size_before = m_history_traffic.Size();
        backup_file.Load();     //Nạp tệp sao lưu (sẽ dọn bản ghi "tương lai")
        size_t backup_size_after_load = backup_file.Size();  //Số bản ghi thực tế sau khi nạp (có thể giảm do dọn bản ghi "tương lai")
        
        // Sau khi nạp, chỉ khôi phục nếu số bản ghi trong tệp sao lưu vẫn lớn hơn tệp hiện tại
        if (backup_size_after_load > m_history_traffic.Size())
        {
            m_history_traffic.Merge(backup_file, true);
            size_t size_after = m_history_traffic.Size();
            size_t recovered_count = size_after - size_before;  //Số bản ghi thực tế đã khôi phục
            
            // Chỉ ghi nhật ký khi thực sự có khôi phục bản ghi
            if (recovered_count > 0)
            {
                CString log_info = CCommon::LoadTextFormat(IDS_HISTORY_TRAFFIC_LOST_ERROR_LOG, { size_before, recovered_count });
                CCommon::WriteLog(log_info, theApp.m_log_path.c_str());
            }
        }
    }

    theApp.m_today_up_traffic = m_history_traffic.GetTodayUpTraffic();
    theApp.m_today_down_traffic = m_history_traffic.GetTodayDownTraffic();
}

void CTrafficMonitorDlg::BackupHistoryTrafficFile()
{
    // Đảm bảo tệp đã được lưu xuống đĩa
    wstring latest_file_path = theApp.m_history_traffic_path;
    wstring backup_file_path = latest_file_path + L".bak";
    
    // Kiểm tra tệp hiện tại có tồn tại không
    if (!CCommon::FileExist(latest_file_path.c_str()))
    {
        return; // Tệp hiện tại không tồn tại, không cần sao lưu
    }
    
    // Sao lưu trực tiếp tệp hiện tại (tệp hiện tại là mới nhất, chứa dữ liệu mới nhất)
    // Tệp sao lưu có thể chứa bản ghi "tương lai", nhưng khi khôi phục sẽ tự động dọn, nên cứ sao lưu tệp hiện tại là được
    CopyFile(latest_file_path.c_str(), backup_file_path.c_str(), FALSE);
}

void CTrafficMonitorDlg::_OnOptions(int tab, CWnd* pParent)
{
    COptionsDlg optionsDlg(tab, pParent);

    //Truyền dữ liệu thiết lập cho hộp thoại Tùy chọn
    if (COptionsDlg::GetUniqueHandel(OPTION_DLG_NAME) == NULL)     //Đảm bảo lúc này hộp thoại Tùy chọn đã đóng
    {
        optionsDlg.m_tab1_dlg.m_data = theApp.m_main_wnd_data;
        optionsDlg.m_tab2_dlg.m_data = theApp.m_taskbar_data;
        optionsDlg.m_tab3_dlg.m_data = theApp.m_general_data;
        optionsDlg.m_tab1_dlg.m_text_disable = m_skin.GetLayoutInfo().no_label;
    }

    if (optionsDlg.DoModal() == IDOK)
    {
        ApplySettings(optionsDlg);
    }
}

void CTrafficMonitorDlg::ApplySettings(COptionsDlg& optionsDlg)
{
    bool is_hardware_monitor_item_changed = (optionsDlg.m_tab3_dlg.m_data.hardware_monitor_item != theApp.m_general_data.hardware_monitor_item);
    bool is_always_on_top_changed = (optionsDlg.m_tab1_dlg.m_data.m_always_on_top != theApp.m_main_wnd_data.m_always_on_top);
    bool is_mouse_penerate_changed = (optionsDlg.m_tab1_dlg.m_data.m_mouse_penetrate != theApp.m_main_wnd_data.m_mouse_penetrate);
    bool is_alow_out_of_border_changed = (optionsDlg.m_tab1_dlg.m_data.m_alow_out_of_border != theApp.m_main_wnd_data.m_alow_out_of_border);
    bool is_show_notify_icon_changed = (optionsDlg.m_tab3_dlg.m_data.show_notify_icon != theApp.m_general_data.show_notify_icon);
    bool is_connections_hide_changed = (optionsDlg.m_tab3_dlg.m_data.connections_hide.data() != theApp.m_general_data.connections_hide.data());
    bool d2d_turned_on = (theApp.m_taskbar_data.disable_d2d && !optionsDlg.m_tab2_dlg.m_data.disable_d2d);
    //Các trường hợp cần đóng rồi mở lại cửa sổ thanh tác vụ
    bool taskbar_changed = (theApp.m_taskbar_data.show_taskbar_wnd_in_secondary_display != optionsDlg.m_tab2_dlg.m_data.show_taskbar_wnd_in_secondary_display
        || theApp.m_taskbar_data.secondary_display_index != optionsDlg.m_tab2_dlg.m_data.secondary_display_index
        || theApp.m_taskbar_data.disable_d2d != optionsDlg.m_tab2_dlg.m_data.disable_d2d
        || theApp.m_taskbar_data.IsTaskbarTransparent() != optionsDlg.m_tab2_dlg.m_data.IsTaskbarTransparent()
        || theApp.m_taskbar_data.auto_set_background_color != optionsDlg.m_tab2_dlg.m_data.auto_set_background_color
        );
    bool is_skin_data_changed = (theApp.m_main_wnd_data.ToSkinSettingData() != optionsDlg.m_tab1_dlg.m_data.ToSkinSettingData());

    theApp.m_main_wnd_data = optionsDlg.m_tab1_dlg.m_data;
    theApp.m_taskbar_data = optionsDlg.m_tab2_dlg.m_data;
    theApp.m_general_data = optionsDlg.m_tab3_dlg.m_data;
    theApp.SendSettingsToPlugin();

    CGeneralSettingsDlg::CheckTaskbarDisplayItem();

    SetTextFont();

    //Sau khi bật kết xuất D2D thì tự động bật “Nền trong suốt” và tắt “Tự động đặt màu nền theo màu thanh tác vụ”
    if (d2d_turned_on)
    {
        theApp.m_taskbar_data.SetTaskabrTransparent(true);
        theApp.m_taskbar_data.auto_set_background_color = false;
    }

    //CTaskBarDlg::SaveConfig();
    if (IsTaskbarWndValid())
    {
        m_tBarDlg->ApplySettings();
        //Nếu đã thay đổi phông chữ hoặc văn bản hiển thị của cửa sổ thanh tác vụ thì cửa sổ thanh tác vụ có thể thay đổi, vì vậy đóng rồi mở lại cửa sổ thanh tác vụ
        if (taskbar_changed)
        {
            CloseTaskBarWnd();
            OpenTaskBarWnd();
        }
        else
        {
            m_tBarDlg->WidthChanged();
        }
        m_tBarDlg->ApplyWindowTransparentColor();
    }

    if (optionsDlg.m_tab3_dlg.IsAutoRunModified())
    {
        if (!theApp.SetAutoRun(theApp.m_general_data.auto_run, theApp.m_general_data.auto_run_by_task_scheduler))
            MessageBox(CCommon::LoadText(IDS_SET_AUTO_RUN_FAILED_WARNING), NULL, MB_ICONWARNING | MB_OK);
    }

    if (optionsDlg.m_tab3_dlg.IsShowAllInterfaceModified() || is_connections_hide_changed)
        IniConnection();

    if (optionsDlg.m_tab3_dlg.IsMonitorTimeSpanModified())      //Nếu khoảng thời gian giám sát thay đổi thì đặt lại bộ định thời
    {
        KillTimer(MONITOR_TIMER);
        SetTimer(MONITOR_TIMER, theApp.m_general_data.monitor_time_span, NULL);
    }

#ifndef WITHOUT_TEMPERATURE
    if (is_hardware_monitor_item_changed)
    {
        //Nếu đã tắt giám sát phần cứng thì hủy lớp giám sát phần cứng
        if (theApp.m_general_data.hardware_monitor_item == 0)
        {
            CSingleLock sync(&theApp.m_minitor_lib_critical, TRUE);
            theApp.m_pMonitor.reset();
        }
        else if (theApp.m_pMonitor != nullptr)
        {
            theApp.UpdateOpenHardwareMonitorEnableState();
        }
        else if (IsTemperatureNeeded())
        {
            theApp.InitOpenHardwareLibInThread();
        }
        //Cập nhật menu chuột phải của cửa sổ thanh tác vụ
        //theApp.UpdateTaskbarWndMenu();
    }
#endif

    if (is_always_on_top_changed)
    {
        SetAlwaysOnTop();
    }

    if (is_mouse_penerate_changed)
    {
        SetMousePenetrate();
        if (!theApp.m_general_data.show_notify_icon && theApp.IsForceShowNotifyIcon())   //Khi cho chuột xuyên qua, nếu biểu tượng thông báo chưa hiển thị thì hiển thị nó, nếu không sẽ không mở được menu chuột phải
        {
            //Thêm biểu tượng vùng thông báo
            AddNotifyIcon();
            theApp.m_general_data.show_notify_icon = true;
        }
    }

    if (is_alow_out_of_border_changed)
    {
        CheckWindowPos();
    }

    if (is_show_notify_icon_changed)
    {
        if (theApp.IsForceShowNotifyIcon())
            theApp.m_general_data.show_notify_icon = true;
        if (theApp.m_general_data.show_notify_icon)
            AddNotifyIcon();
        else
            DeleteNotifyIcon();
    }

    if (is_skin_data_changed)
    {
        //Lưu thiết lập skin hiện tại vào SkinManager
        SkinSettingData skin_data = theApp.m_main_wnd_data.ToSkinSettingData();
        CSkinManager::Instance().AddSkinSettingData(theApp.m_cfg_data.m_skin_name, skin_data);
        CSkinManager::Instance().Save();
    }

    theApp.SaveConfig();
    theApp.SaveGlobalConfig();
}

void CTrafficMonitorDlg::SetItemPosition()
{
    if (theApp.m_cfg_data.m_show_more_info)
    {
        SetWindowPos(nullptr, 0, 0, m_skin.GetLayoutInfo().layout_l.width, m_skin.GetLayoutInfo().layout_l.height, SWP_NOMOVE | SWP_NOZORDER);
    }
    else
    {
        SetWindowPos(nullptr, 0, 0, m_skin.GetLayoutInfo().layout_s.width, m_skin.GetLayoutInfo().layout_s.height, SWP_NOMOVE | SWP_NOZORDER);
    }
}

bool CTrafficMonitorDlg::LoadSkinLayout()
{
    bool rtn = m_skin.Load(CSkinManager::Instance().GetSkinName(m_skin_selected));
    if (m_skin.GetLayoutInfo().no_label)        //Nếu bố cục skin không hiển thị văn bản thì không cho đổi chỗ tải lên và tải xuống, vì vị trí tải lên và tải xuống đã cố định trong skin
        theApp.m_main_wnd_data.swap_up_down = false;
    return rtn;
}

void CTrafficMonitorDlg::LoadBackGroundImage()
{
    CImage img_tmp;
    CSize image_size;
    if (theApp.m_cfg_data.m_show_more_info)
    {
        image_size.SetSize(m_skin.GetLayoutInfo().layout_l.width, m_skin.GetLayoutInfo().layout_l.height);
    }
    else
    {
        image_size.SetSize(m_skin.GetLayoutInfo().layout_s.width, m_skin.GetLayoutInfo().layout_s.height);
    }

    //Tạo vùng cửa sổ
    CImage img_mask;
    //Nạp ảnh mặt nạ
    if (theApp.m_cfg_data.m_show_more_info)
        img_tmp.Load((theApp.m_skin_path + CSkinManager::Instance().GetSkinName(m_skin_selected) + BACKGROUND_MASK_L).c_str());
    else
        img_tmp.Load((theApp.m_skin_path + CSkinManager::Instance().GetSkinName(m_skin_selected) + BACKGROUND_MASK_S).c_str());
    CRgn wnd_rgn;
    if (!img_tmp.IsNull())
    {
        CDrawCommon::BitmapStretch(&img_tmp, &img_mask, image_size);        //Co giãn ảnh mặt nạ
        CBitmap bitmap;
        bitmap.Attach(img_mask);
        CDrawCommon::GetRegionFromImage(wnd_rgn, bitmap, 128);      //Lấy vùng cửa sổ từ ảnh mặt nạ (mask)
        bitmap.Detach();
    }
    else
    {
        wnd_rgn.CreateRectRgnIndirect(CRect(CPoint(0, 0), image_size));     //Nạp ảnh mặt nạ thất bại thì dùng kích thước cửa sổ làm vùng cửa sổ
    }
    //Tránh trường hợp vùng cửa sổ lấy được bị rỗng
    CRgn empty_rgn;
    empty_rgn.CreateRectRgnIndirect(CRect{});   //Tạo một đối tượng CRgn rỗng
    if (wnd_rgn.EqualRgn(&empty_rgn))
        wnd_rgn.SetRectRgn(CRect(CPoint(0, 0), image_size));    //Nếu vùng cửa sổ lấy được rỗng thì dùng kích thước cửa sổ làm vùng cửa sổ
    SetWindowRgn(wnd_rgn, TRUE);        //Thiết lập vùng cửa sổ
    wnd_rgn.DeleteObject();
    empty_rgn.DeleteObject();
}

void CTrafficMonitorDlg::SetTextFont()
{
    m_skin.SetSettingData(theApp.m_main_wnd_data.ToSkinSettingData());
}

bool CTrafficMonitorDlg::IsTaskbarWndValid() const
{
    return m_tBarDlg != nullptr && ::IsWindow(m_tBarDlg->GetSafeHwnd());
}

void CTrafficMonitorDlg::TaskbarShowHideItem(DisplayItem type)
{
    if (IsTaskbarWndValid())
    {
        bool show = (theApp.m_taskbar_data.display_item.Contains(type));
        if (show)
        {
            theApp.m_taskbar_data.display_item.Remove(type);
        }
        else
        {
            theApp.m_taskbar_data.display_item.Add(type);
        }
        //CloseTaskBarWnd();
        //OpenTaskBarWnd();
        m_tBarDlg->WidthChanged();
    }
}

void CTrafficMonitorDlg::CheckClickedItem(CPoint point)
{
    const CSkinFile::Layout& skin_layout{ theApp.m_cfg_data.m_show_more_info ? m_skin.GetLayoutInfo().layout_l : m_skin.GetLayoutInfo().layout_s }; //Bố cục skin hiện tại
    for (const auto& layout_item : skin_layout.layout_items)
    {
        CRect rect(CPoint(layout_item.second.x, layout_item.second.y), CSize(layout_item.second.width, m_skin.GetLayoutInfo().text_height));
        if (rect.PtInRect(point))
        {
            m_clicked_item = layout_item.first;
            break;
        }
    }
}

void CTrafficMonitorDlg::ApplySkin(int skin_index)
{
    if (skin_index < 0 || skin_index >= CSkinManager::Instance().Size())
        return;
    m_skin_selected = skin_index;
    theApp.m_cfg_data.m_skin_name = CSkinManager::Instance().GetSkinName(m_skin_selected);
    //Lấy bố cục skin
    LoadSkinLayout();
    //Nạp ảnh nền
    LoadBackGroundImage();
    //Lấy thiết lập skin
    SkinSettingData cur_skin_data;
    if (!CSkinManager::Instance().GetSkinSettingDataByIndex(m_skin_selected, cur_skin_data))
    {
        CSkinManager::SkinSettingDataFronSkin(cur_skin_data, m_skin);
    }
    theApp.m_main_wnd_data.FormSkinSettingData(cur_skin_data);
    SetTextFont();
    SetItemPosition();
    Invalidate(FALSE);      //Làm mới thông tin cửa sổ ngay sau khi đổi skin
    //Thiết lập lại kiểu WS_EX_LAYERED để khắc phục hiển thị bất thường khi chuyển giữa skin png và skin bmp
    //Xóa kiểu phân lớp (layered) của cửa sổ
    SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
    //Thiết lập lại độ trong suốt
    SetTransparency();

    theApp.SaveConfig();
}

bool CTrafficMonitorDlg::IsTemperatureNeeded() const
{
    //Kiểm tra có cần lấy thông tin từ OpenHardwareMonitor hay không.
    ////Chỉ trả về true khi cửa sổ chính và cửa sổ thanh tác vụ có ít nhất một mục cần hiển thị trong số: nhiệt độ CPU, mức sử dụng GPU, nhiệt độ GPU, nhiệt độ ổ cứng và nhiệt độ bo mạch chủ
    //bool needed = false;
    //if (theApp.m_cfg_data.m_show_task_bar_wnd && IsTaskbarWndValid())
    //{
    //    needed |= m_tBarDlg->IsShowCpuTemperature();
    //    needed |= m_tBarDlg->IsShowGpu();
    //    needed |= m_tBarDlg->IsShowGpuTemperature();
    //    needed |= m_tBarDlg->IsShowHddTemperature();
    //    needed |= m_tBarDlg->IsShowMainboardTemperature();
    //    needed |= (::IsWindow(m_tBarDlg->m_tool_tips.GetSafeHwnd()) && m_tBarDlg->m_tool_tips.IsWindowVisible());
    //}

    //if (!theApp.m_cfg_data.m_hide_main_window)
    //{
    //    const CSkinFile::Layout& skin_layout{ theApp.m_cfg_data.m_show_more_info ? m_skin.GetLayoutInfo().layout_l : m_skin.GetLayoutInfo().layout_s }; //Bố cục skin hiện tại
    //    needed |= skin_layout.GetItem(TDI_CPU_TEMP).show;
    //    needed |= skin_layout.GetItem(TDI_GPU_USAGE).show;
    //    needed |= skin_layout.GetItem(TDI_GPU_TEMP).show;
    //    needed |= skin_layout.GetItem(TDI_HDD_TEMP).show;
    //    needed |= skin_layout.GetItem(TDI_MAIN_BOARD_TEMP).show;
    //    needed |= (::IsWindow(m_tool_tips.GetSafeHwnd()) && m_tool_tips.IsWindowVisible());
    //}
    //return needed;

    return theApp.m_general_data.IsHardwareEnable(HI_CPU) || theApp.m_general_data.IsHardwareEnable(HI_GPU)
        || theApp.m_general_data.IsHardwareEnable(HI_HDD) || theApp.m_general_data.IsHardwareEnable(HI_MBD);
}

// Trình xử lý thông điệp của CTrafficMonitorDlg

BOOL CTrafficMonitorDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Thiết lập biểu tượng cho hộp thoại này. Khi cửa sổ chính của ứng dụng không phải hộp thoại, framework sẽ tự động
    //  Thực hiện thao tác này
    SetIcon(m_hIcon, TRUE);         // Thiết lập biểu tượng lớn
    SetIcon(m_hIcon, FALSE);        // Thiết lập biểu tượng nhỏ

    // TODO: Thêm mã khởi tạo bổ sung tại đây
    SetWindowText(APP_NAME);
    //Thiết lập ẩn biểu tượng trên thanh tác vụ
    ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

    theApp.DPIFromWindow(this);
    //Lấy kích thước màn hình
    GetScreenSize();
    m_last_screen_rects = m_screen_rects;
    //::SystemParametersInfo(SPI_GETWORKAREA, 0, &m_screen_rect, 0);   // Lấy kích thước vùng làm việc

    //Khởi tạo menu
    theApp.InitMenuResourse();
    //theApp.UpdateTaskbarWndMenu();

    //Khởi tạo skin
    CSkinManager::Instance().Init();
    m_skin_selected = CSkinManager::Instance().FindSkinIndex(theApp.m_cfg_data.m_skin_name);

    //Lấy dữ liệu bố cục theo skin đang chọn
    if (LoadSkinLayout())
    {
        //Lấy thiết lập của skin hiện tại từ SkinManager
        SkinSettingData cur_skin_data;
        CSkinManager::SkinSettingDataFronSkin(cur_skin_data, m_skin);   //Lấy thiết lập mặc định của skin
        CSkinManager::Instance().GetSkinSettingDataByIndex(m_skin_selected, cur_skin_data); //Lấy dữ liệu người dùng đã lưu của skin
        theApp.m_main_wnd_data.FormSkinSettingData(cur_skin_data);
    }

    //Thiết lập độ trong suốt cửa sổ
    SetTransparency();

    IniConnection();    //Khởi tạo kết nối

    //Nếu khi khởi động đã bật cho chuột xuyên qua hoặc ẩn cửa sổ chính, và không hiển thị cửa sổ thanh tác vụ, thì hiển thị biểu tượng vùng thông báo
    if ((theApp.m_main_wnd_data.m_mouse_penetrate || theApp.m_cfg_data.m_hide_main_window) && !theApp.m_cfg_data.m_show_task_bar_wnd)
        theApp.m_general_data.show_notify_icon = true;

    //Nạp biểu tượng vùng thông báo
    theApp.m_notify_icons[0] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_NOFITY_ICON), IMAGE_ICON, theApp.DPI(16), theApp.DPI(16), LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
    theApp.m_notify_icons[1] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_NOFITY_ICON2), IMAGE_ICON, theApp.DPI(16), theApp.DPI(16), LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
    theApp.m_notify_icons[2] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_NOFITY_ICON3), IMAGE_ICON, theApp.DPI(16), theApp.DPI(16), LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
    theApp.m_notify_icons[3] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, theApp.DPI(16), theApp.DPI(16), LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
    theApp.m_notify_icons[4] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_NOFITY_ICON4), IMAGE_ICON, theApp.DPI(16), theApp.DPI(16), LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
    theApp.m_notify_icons[5] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_NOTIFY_ICON5), IMAGE_ICON, theApp.DPI(16), theApp.DPI(16), LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);

    //Thiết lập biểu tượng vùng thông báo
    m_ntIcon.cbSize = sizeof(NOTIFYICONDATA);   //Kích thước của biến cấu trúc này
    if (theApp.m_cfg_data.m_notify_icon_selected < 0 || theApp.m_cfg_data.m_notify_icon_selected >= MAX_NOTIFY_ICON)
        theApp.m_cfg_data.m_notify_icon_selected = 0;
    m_ntIcon.hIcon = theApp.m_notify_icons[theApp.m_cfg_data.m_notify_icon_selected];       //Thiết lập biểu tượng
    m_ntIcon.hWnd = this->m_hWnd;               //Handle cửa sổ nhận thông điệp thông báo của biểu tượng khay hệ thống
    CString atip;           //Chú giải hiển thị khi trỏ chuột vào biểu tượng
#ifdef _DEBUG
    atip = CCommon::LoadText(IDS_TRAFFICMONITOR, _T(" (Debug)"));
#else
    atip = CCommon::LoadText(IDS_TRAFFICMONITOR);
#endif
    //wcscpy_s(m_ntIcon.szTip, 128, strTip);
    CCommon::WStringCopy(m_ntIcon.szTip, 128, atip.GetString());
    m_ntIcon.uCallbackMessage = MY_WM_NOTIFYICON;   //Mã ID thông điệp do ứng dụng định nghĩa
    m_ntIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; //Thuộc tính biểu tượng: thiết lập các thành viên uCallbackMessage, hIcon, szTip có hiệu lực
    if (theApp.m_general_data.show_notify_icon)
        ::Shell_NotifyIcon(NIM_ADD, &m_ntIcon); //Thêm biểu tượng này vào vùng thông báo hệ thống

    //Tải lịch sử lưu lượng
    LoadHistoryTraffic();

    //Thiết lập bộ định thời kích hoạt mỗi 1000 mili giây
    SetTimer(MAIN_TIMER, 1000, NULL);

    SetTimer(MONITOR_TIMER, theApp.m_general_data.monitor_time_span, NULL);
    AfxBeginThread(MonitorThreadCallback, (LPVOID)this);

    //Khởi tạo vị trí cửa sổ
    SetItemPosition();
    if (theApp.m_cfg_data.m_position_x != -1 && theApp.m_cfg_data.m_position_y != -1)
        SetWindowPos(nullptr, theApp.m_cfg_data.m_position_x, theApp.m_cfg_data.m_position_y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
    CheckWindowPos();

    //Nạp ảnh nền
    LoadBackGroundImage();

    //Thiết lập phông chữ
    SetTextFont();

    //Lấy thời điểm khởi động
    GetLocalTime(&m_start_time);

    //Khởi tạo chú giải chuột
    m_tool_tips.Create(this, TTS_ALWAYSTIP);
    m_tool_tips.SetMaxTipWidth(600);
    m_tool_tips.AddTool(this, _T(""));

    //Nếu khi khởi động chương trình đã thiết lập ẩn cửa sổ chính, hoặc cửa sổ nằm ở góc trên bên trái, thì đặt độ mờ đục bằng 0 trước
    if (theApp.m_cfg_data.m_hide_main_window || (theApp.m_cfg_data.m_position_x == 0 && theApp.m_cfg_data.m_position_y == 0))
        SetTransparency(0);

    SetTimer(TASKBAR_TIMER, 100, NULL);

    return TRUE;  // Trả về TRUE trừ khi đặt tiêu điểm vào điều khiển
}


//Hệ thống gọi hàm này để lấy con trỏ chuột khi người dùng kéo cửa sổ thu nhỏ
//hiển thị.
HCURSOR CTrafficMonitorDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

//Tính số lần bộ định thời Monitor kích hoạt trong số giây chỉ định
static int GetMonitorTimerCount(int second)
{
    int count = second * 1000 / theApp.m_general_data.monitor_time_span;
    if (count <= 0) count = 1;
    return count;
}


//Gọi GetIfTable có bắt ngoại lệ SEH (tách thành hàm riêng vì __try không dùng được trong hàm có đối tượng C++ cần hủy).
//Khi lỗi trả về ERROR_INSUFFICIENT_BUFFER để luồng giao diện cấp phát lại bảng.
static DWORD SafeGetIfTable(MIB_IFTABLE* table, DWORD size)
{
    if (table == nullptr)
        return ERROR_INSUFFICIENT_BUFFER;
    __try
    {
        return GetIfTable(table, &size, FALSE);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return ERROR_INSUFFICIENT_BUFFER;
    }
}

void CTrafficMonitorDlg::DoMonitorAcquisition()
{
    //Lấy tốc độ kết nối mạng.
    //Toàn bộ phần đọc m_pIfTable/m_connections được khóa vì IniConnection (luồng giao diện) có thể cấp phát lại chúng.
    //Luồng giám sát không bao giờ cấp phát lại bảng: khi cần thì gửi yêu cầu cho luồng giao diện (RequestReinitConnection).
    DWORD rtn{};
    bool counters_valid = true;     //false nếu không lấy được bộ đếm 64-bit của một kết nối nào đó
    {
        CSingleLock lock(&m_connections_lock, TRUE);
        rtn = SafeGetIfTable(m_pIfTable, m_dwSize);

        //Dùng bộ đếm 64-bit (GetIfEntry2): bộ đếm 32-bit của MIB_IFROW quay vòng sau mỗi 4 GiB làm mất lưu lượng
        //và làm tốc độ về 0 tại thời điểm quay vòng; khi "Chọn tất cả", một kết nối quay vòng làm mất cả lần đo.
        if (!theApp.m_cfg_data.m_select_all)        //Lấy tốc độ mạng của kết nối đang chọn
        {
            auto table = GetConnectIfTable(m_connection_selected);
            if (!CAdapterCommon::GetInterfaceOctets(table, m_in_bytes, m_out_bytes))
                counters_valid = false;
        }
        else        //Lấy tốc độ mạng của tất cả kết nối
        {
            m_in_bytes = 0;
            m_out_bytes = 0;
            for (int i{}; i < static_cast<int>(m_connections.size()); i++)
            {
                auto table = GetConnectIfTable(i);
                unsigned __int64 in_bytes{}, out_bytes{};
                if (!CAdapterCommon::GetInterfaceOctets(table, in_bytes, out_bytes))
                    counters_valid = false;
                m_in_bytes += in_bytes;
                m_out_bytes += out_bytes;
            }
        }
    }

    unsigned __int64 cur_in_speed{}, cur_out_speed{};       //Tốc độ tải lên và tải xuống trong khoảng thời gian giám sát lần này

    //Nếu số byte gửi và nhận bằng 0, hoặc số byte gửi và nhận lần trước bằng 0, hoặc kết nối hiện tại đã thay đổi, thì tốc độ mạng không hợp lệ
    //Nếu lần này hoặc lần trước phải dùng bộ đếm 32-bit dự phòng thì hai giá trị không so sánh được với nhau
    if ((m_in_bytes == 0 && m_out_bytes == 0) || (m_last_in_bytes == 0 && m_last_out_bytes == 0) || m_connection_change_flag
        || m_last_in_bytes > m_in_bytes || m_last_out_bytes > m_out_bytes || !counters_valid || !m_last_counters_valid)
    {
        cur_in_speed = 0;
        cur_out_speed = 0;
    }
    else
    {
        cur_in_speed = m_in_bytes - m_last_in_bytes;
        cur_out_speed = m_out_bytes - m_last_out_bytes;
    }
    ////Nếu lớn hơn 1GB/s thì có thể đã xảy ra bất thường, tốc độ mạng không hợp lệ
    //if (cur_in_speed > 1073741824)
    //  cur_in_speed = 0;
    //if (cur_out_speed > 1073741824)
    //  cur_out_speed = 0;

    //Tính khoảng thời gian giữa hai lần lấy tốc độ mạng
    static ULONGLONG last_net_speed_time = 0;
    //Dùng GetTickCount64 (đồng hồ đơn điệu): giờ hệ thống có thể bị chỉnh lùi/tiến (đồng bộ NTP, người dùng đổi giờ)
    //làm khoảng thời gian bằng 0 (chia cho 0 gây crash) hoặc âm/rất lớn (tốc độ sai).
    ULONGLONG net_speed_time = GetTickCount64();
    int time_span = theApp.m_general_data.monitor_time_span;
    if (last_net_speed_time != 0 && net_speed_time > last_net_speed_time && net_speed_time - last_net_speed_time < 3600000ULL)
        time_span = static_cast<int>(net_speed_time - last_net_speed_time);
    last_net_speed_time = net_speed_time;
    if (time_span <= 0)
        time_span = 1000;

    //Chuyển lưu lượng trong khoảng thời gian giám sát hiện tại thành lưu lượng trên mỗi giây
    theApp.m_in_speed = static_cast<unsigned __int64>(cur_in_speed * 1000 / time_span);
    theApp.m_out_speed = static_cast<unsigned __int64>(cur_out_speed * 1000 / time_span);

    m_connection_change_flag = false;    //Xóa cờ báo kết nối đã thay đổi

    m_last_in_bytes = m_in_bytes;
    m_last_out_bytes = m_out_bytes;
    m_last_counters_valid = counters_valid;

    //Khi đang ở trạng thái tự động chọn, nếu liên tục 30 giây không có tốc độ mạng thì có thể mạng được tự động chọn không đúng, lúc này thực hiện tự động chọn lại một lần
    if (theApp.m_cfg_data.m_auto_select)
    {
        if (cur_in_speed == 0 && cur_out_speed == 0)
            m_zero_speed_cnt++;
        else
            m_zero_speed_cnt = 0;
        if (m_zero_speed_cnt >= GetMonitorTimerCount(30))
        {
            AutoSelect();
            m_zero_speed_cnt = 0;
        }
    }

    //Kiểm tra ngày hiện tại có thay đổi không; nếu đã thay đổi thì chèn một ngày mới vào danh sách lưu lượng lịch sử
    SYSTEMTIME current_time;
    GetLocalTime(&current_time);
    static int last_check_day = -1;  //Dùng để phát hiện thay đổi ngày, đặt lại trạng thái lưu
    if (m_history_traffic.GetTodayTraffic().day != current_time.wDay)
    {
        m_history_traffic.OnDateChanged();
        theApp.m_today_up_traffic = 0;
        theApp.m_today_down_traffic = 0;
        last_check_day = -1;  //Đặt lại dấu ngày, lần kiểm tra sau sẽ khởi tạo lại trạng thái lưu
    }

    //Thống kê lưu lượng đã dùng hôm nay
    theApp.m_today_up_traffic += cur_out_speed;
    theApp.m_today_down_traffic += cur_in_speed;
    m_history_traffic.GetTodayTraffic().up_kBytes = theApp.m_today_up_traffic / 1024u;
    m_history_traffic.GetTodayTraffic().down_kBytes = theApp.m_today_down_traffic / 1024u;
    //Cứ 30 giây lưu lịch sử lưu lượng một lần
    if (m_monitor_time_cnt % GetMonitorTimerCount(30) == GetMonitorTimerCount(30) - 1)
    {
        static unsigned __int64 last_today_kbytes = 0;
        static bool last_today_kbytes_initialized = false;
        unsigned __int64 current_kbytes = m_history_traffic.GetTodayTraffic().kBytes();
        
        //Nếu ngày đã thay đổi thì đặt lại trạng thái khởi tạo
        if (last_check_day != current_time.wDay)
        {
            last_today_kbytes_initialized = false;
            last_check_day = current_time.wDay;
        }
        
        //Khởi tạo khi kiểm tra lần đầu, không lưu
        if (!last_today_kbytes_initialized)
        {
            last_today_kbytes = current_kbytes;
            last_today_kbytes_initialized = true;
        }
        else
        {
            //Chỉ lưu bản ghi lưu lượng lịch sử khi lưu lượng thay đổi quá 10MB trong 30 giây, tránh ghi đĩa quá thường xuyên
            unsigned __int64 change_kbytes = current_kbytes - last_today_kbytes;
            if (change_kbytes >= 10240u) // 10MB = 10240KB
            {
                SaveHistoryTraffic();
                last_today_kbytes = current_kbytes;
            }
        }
    }

    if (rtn == ERROR_INSUFFICIENT_BUFFER)
        RequestReinitConnection(REINIT_INSUFFICIENT_BUFFER);

    if (m_monitor_time_cnt % GetMonitorTimerCount(3) == GetMonitorTimerCount(3) - 1)
    {
        //Lấy lại số lượng kết nối hiện tại
        static DWORD last_interface_num = static_cast<DWORD>(-1);
        DWORD interface_num{};
        if (GetNumberOfInterfaces(&interface_num) != NO_ERROR)
            interface_num = last_interface_num;
        if (last_interface_num != static_cast<DWORD>(-1) && interface_num != last_interface_num)    //Nếu số kết nối thay đổi thì khởi tạo lại kết nối
        {
            if (theApp.m_debug_log)
            {
                CString info = CCommon::LoadText(IDS_CONNECTION_NUM_CHANGED);
                info.Replace(_T("<%before%>"), CCommon::IntToString(last_interface_num));
                info.Replace(_T("<%after%>"), CCommon::IntToString(interface_num));
                info.Replace(_T("<%cnt%>"), CCommon::IntToString(m_restart_cnt + 1));
                CCommon::WriteLog(info, theApp.m_log_path.c_str());
            }
            RequestReinitConnection(REINIT_INTERFACE_NUM_CHANGED);
        }
        //Lỗi cũ: biến này chỉ được gán bên trong nhánh if ở trên nên luôn bằng -1, khiến thay đổi số kết nối không bao giờ được phát hiện
        last_interface_num = interface_num;

        string descr;
        {
            CSingleLock lock(&m_connections_lock, TRUE);
            descr = CAdapterCommon::GetIfRowDescription(GetConnectIfTable(m_connection_selected));
        }
        if (descr != theApp.m_cfg_data.m_connection_name)
        {
            //Ghi thêm thông tin gỡ lỗi
            if (theApp.m_debug_log)
            {
                CString log_str;
                log_str = _T("Tên kết nối không khớp:\r\n");
                log_str += _T("IfTable description: ");
                log_str += descr.c_str();
                log_str += _T("\r\nm_connection_name: ");
                log_str += theApp.m_cfg_data.m_connection_name.c_str();
                CCommon::WriteLog(log_str, (theApp.m_config_dir + L".\\connections.log").c_str());
            }

            RequestReinitConnection(REINIT_CONNECTION_NOT_MATCH);
        }
    }

    bool lite_version = false;
#ifdef WITHOUT_TEMPERATURE
    lite_version = true;
#endif

    bool cpu_freq_acquired = false;
    bool gpu_usage_acquired = false;
    m_get_disk_usage_by_pdh = false;

    //Lấy mức sử dụng CPU
    theApp.m_cpu_usage = m_cpu_usage_helper.GetCpuUsage(theApp.m_general_data.cpu_usage_acquire_method == GeneralSettingData::CA_CPU_TIME);

    //Lấy tần số CPU
    //if (lite_version || is_arm64ec || !theApp.m_general_data.IsHardwareEnable(HI_CPU))
    //{
    if (m_cpu_freq_helper.GetCpuFreq(theApp.m_cpu_freq))
        cpu_freq_acquired = true;
    //}

    //Lấy mức sử dụng GPU
    if (lite_version /*|| is_arm64ec*/ || !theApp.m_general_data.IsHardwareEnable(HI_GPU))
    {
        if (m_gpu_usage_helper.GetGpuUsage(theApp.m_gpu_usage))
            gpu_usage_acquired = true;
        else
            theApp.m_gpu_usage = -1;
    }

    //Lấy mức sử dụng ổ cứng
    if (lite_version /*|| is_arm64ec*/ || !theApp.m_general_data.IsHardwareEnable(HI_HDD))
    {
        int disk_index = m_disk_usage_helper.FindDiskIndex(theApp.m_general_data.hard_disk_name);
        //Khi không tìm thấy ổ cứng cần giám sát thì mặc định dùng mức sử dụng tổng thể
        if (disk_index < 0)
        {
            disk_index = m_disk_usage_helper.FindDiskIndex(L"_Total");
            if (disk_index >= 0)
            {
                theApp.m_general_data.hard_disk_name = L"_Total";
            }
            //Vẫn không tìm thấy thì dùng ổ cứng thứ 1
            else
            {
                const auto& disk_names = m_disk_usage_helper.GetDiskNames();
                if (!disk_names.empty())
                {
                    disk_index = 0;
                    theApp.m_general_data.hard_disk_name = disk_names.front();
                }
            }
        }
        if (m_disk_usage_helper.GetDiskUsage(disk_index, theApp.m_hdd_usage))
            m_get_disk_usage_by_pdh = true;
        else
            theApp.m_hdd_usage = -1;
    }

    //Lấy mức sử dụng bộ nhớ
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    theApp.m_memory_usage = statex.dwMemoryLoad;
    theApp.m_used_memory = static_cast<int>((statex.ullTotalPhys - statex.ullAvailPhys) / 1024);
    theApp.m_total_memory = static_cast<int>(statex.ullTotalPhys / 1024);

#ifndef WITHOUT_TEMPERATURE
    //Lấy nhiệt độ
    if (IsTemperatureNeeded() && theApp.m_pMonitor != nullptr)
    {
        CSingleLock sync(&theApp.m_minitor_lib_critical, TRUE);
        CString error_info = CCommon::LoadText(IDS_HARDWARE_INFO_ACQUIRE_FAILED_ERROR);

        auto getHardwareInfo = [&]() {
            __try
            {
                theApp.m_pMonitor->GetHardwareInfo();
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                AfxMessageBox(error_info, MB_ICONERROR | MB_OK);
            }
        };

        getHardwareInfo();
        auto monitor_error_message{ OpenHardwareMonitorApi::GetErrorMessage() };
        if (!monitor_error_message.empty())
        {
            AfxMessageBox(monitor_error_message.c_str(), MB_ICONERROR | MB_OK);
        }
        //theApp.m_cpu_temperature = theApp.m_pMonitor->CpuTemperature();
        theApp.m_gpu_temperature = theApp.m_pMonitor->GpuTemperature();
        //theApp.m_hdd_temperature = theApp.m_pMonitor->HDDTemperature();
        theApp.m_main_board_temperature = theApp.m_pMonitor->MainboardTemperature();
        if (!gpu_usage_acquired)
            theApp.m_gpu_usage = theApp.m_pMonitor->GpuUsage();
        if (!cpu_freq_acquired)
            theApp.m_cpu_freq = theApp.m_pMonitor->CpuFreq();
        //Lấy nhiệt độ CPU
        if (!theApp.m_pMonitor->AllCpuTemperature().empty())
        {
            if (theApp.m_general_data.cpu_core_name == CCommon::LoadText(IDS_AVREAGE_TEMPERATURE).GetString())  //Nếu đã chọn nhiệt độ trung bình
            {
                theApp.m_cpu_temperature = theApp.m_pMonitor->CpuTemperature();
            }
            else
            {
                auto iter = theApp.m_pMonitor->AllCpuTemperature().find(theApp.m_general_data.cpu_core_name);
                if (iter == theApp.m_pMonitor->AllCpuTemperature().end())
                {
                    iter = theApp.m_pMonitor->AllCpuTemperature().begin();
                    theApp.m_general_data.cpu_core_name = iter->first;
                }
                theApp.m_cpu_temperature = iter->second;
            }
        }
        else
        {
            theApp.m_cpu_temperature = -1;
        }
        //Lấy nhiệt độ ổ cứng
        if (!theApp.m_pMonitor->AllHDDTemperature().empty())
        {
            auto iter = theApp.m_pMonitor->AllHDDTemperature().find(theApp.m_general_data.hard_disk_name);
            if (iter == theApp.m_pMonitor->AllHDDTemperature().end())
            {
                iter = theApp.m_pMonitor->AllHDDTemperature().begin();
                theApp.m_general_data.hard_disk_name = iter->first;
            }
            theApp.m_hdd_temperature = iter->second;
        }
        else
        {
            theApp.m_hdd_temperature = -1;
        }
        //Lấy mức sử dụng ổ cứng
        if (!m_get_disk_usage_by_pdh)
        {
            if (!theApp.m_pMonitor->AllHDDUsage().empty())
            {
                auto iter = theApp.m_pMonitor->AllHDDUsage().find(theApp.m_general_data.hard_disk_name);
                if (iter == theApp.m_pMonitor->AllHDDUsage().end())
                {
                    iter = theApp.m_pMonitor->AllHDDUsage().begin();
                    theApp.m_general_data.hard_disk_name = iter->first;
                }
                theApp.m_hdd_usage = iter->second;
            }
            else
            {
                theApp.m_hdd_usage = -1;
            }
        }
    }
#endif

    //Báo plugin lấy dữ liệu, đồng thời truyền dữ liệu giám sát cho plugin
    for (const auto& plugin_info : theApp.m_plugins.GetPlugins())
    {
        if (plugin_info.plugin != nullptr)
        {
            plugin_info.plugin->DataRequired();
            ITMPlugin::MonitorInfo monitor_info;
            monitor_info.up_speed = theApp.m_out_speed;
            monitor_info.down_speed = theApp.m_in_speed;
            monitor_info.cpu_usage = theApp.m_cpu_usage;
            monitor_info.memory_usage = theApp.m_memory_usage;
            monitor_info.gpu_usage = theApp.m_gpu_usage;
            monitor_info.hdd_usage = theApp.m_hdd_usage;
            //MonitorInfo của giao diện plugin dùng số nguyên (giữ nguyên ABI): cắt phần thập phân như trước
            monitor_info.cpu_temperature = static_cast<int>(theApp.m_cpu_temperature);
            monitor_info.gpu_temperature = static_cast<int>(theApp.m_gpu_temperature);
            monitor_info.hdd_temperature = static_cast<int>(theApp.m_hdd_temperature);
            monitor_info.cpu_freq = static_cast<int>(theApp.m_cpu_freq);
            monitor_info.main_board_temperature = static_cast<int>(theApp.m_main_board_temperature);
            plugin_info.plugin->OnMonitorInfo(monitor_info);
        }
    }

    m_monitor_time_cnt++;

    //Gửi thông điệp cập nhật thông tin giám sát
    SendMessage(WM_MONITOR_INFO_UPDATED);
}

UINT CTrafficMonitorDlg::MonitorThreadCallback(LPVOID dwUser)
{
    CTrafficMonitorDlg* pThis = (CTrafficMonitorDlg*)dwUser;
    while (true)
    {
        //Lấy dữ liệu giám sát một lần
        if (pThis->m_monitor_data_required)
        {
            pThis->DoMonitorAcquisition();
            //Đặt lại flag sau khi lấy được dữ liệu giám sát
            pThis->m_monitor_data_required = false;
        }
        else
        {
            Sleep(10);
        }

        // Kiểm tra cờ thoát
        if (pThis->m_is_thread_exit)
        {
            // Kích hoạt sự kiện, báo cho luồng chính biết luồng làm việc đã thoát
            pThis->m_threadExitEvent.SetEvent();
            return 0;
        }
    }

    return 0;
}


void CTrafficMonitorDlg::ExitMonitorThread()
{
    // Báo luồng thoát
    m_is_thread_exit = true;

    // Chờ luồng thoát
    ::WaitForSingleObject(m_threadExitEvent.m_hObject, 1000);
}


void CTrafficMonitorDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây
    if (nIDEvent == MONITOR_TIMER)
    {
        //Báo luồng lấy dữ liệu giám sát
        m_monitor_data_required = true;
    }

    if (nIDEvent == MAIN_TIMER)
    {
        m_timer_cnt++;
        if (m_first_start)      //Câu lệnh if này chạy 1 giây sau khi chương trình khởi động
        {
            //Đặt xử lý ghim cửa sổ trên cùng ở đây là để giải quyết
            //vấn đề đặt trong hàm khởi tạo có thể khiến việc ghim trên cùng không có tác dụng
            SetAlwaysOnTop();       //Ghim cửa sổ trên cùng
            SetMousePenetrate();    //Thiết lập cho chuột xuyên qua
            if (theApp.m_cfg_data.m_hide_main_window)   //Thiết lập ẩn cửa sổ chính
                ShowWindow(SW_HIDE);

            //Mở cửa sổ thanh tác vụ
            if (theApp.m_cfg_data.m_show_task_bar_wnd && m_tBarDlg == nullptr)
                OpenTaskBarWnd();

            //Nếu vị trí cửa sổ là (0, 0) thì khi khởi tạo hàm MoveWindow không có tác dụng, lúc này di chuyển cửa sổ thêm một lần
            if (theApp.m_cfg_data.m_position_x == 0 && theApp.m_cfg_data.m_position_y == 0)
            {
                SetWindowPos(nullptr, theApp.m_cfg_data.m_position_x, theApp.m_cfg_data.m_position_y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
            }
            SetTransparency();              //Thiết lập lại độ mờ đục cửa sổ

            //Kiểm tra DPI tại vị trí cửa sổ chính có thay đổi không
            UINT dpi_x, dpi_y;
            CRect rect;
            GetWindowRect(rect);
            bool rtn = theApp.DPIFromRect(rect, & dpi_x, & dpi_y);
            int dpi = static_cast<int>(dpi_x);
            if (rtn && dpi != theApp.GetDpi())
            {
                theApp.SetDPI(dpi_x);
                LoadSkinLayout();   //Lấy dữ liệu bố cục theo skin đang chọn
                SetItemPosition();  //Khởi tạo vị trí cửa sổ
                LoadBackGroundImage();
                SetTextFont();      //Nạp lại phông chữ
                Invalidate(FALSE);  //Vẽ lại giao diện
            }

            m_first_start = false;
        }

        if (theApp.m_main_wnd_data.m_always_on_top && !theApp.m_cfg_data.m_hide_main_window)
        {
            //Cứ 1 giây kiểm tra một lần xem cửa sổ tiền cảnh có toàn màn hình không
            CRect rect;
            GetWindowRect(rect);
            HMONITOR h_current_monitor = ::MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);
            m_is_foreground_fullscreen = CCommon::IsForegroundFullscreen(h_current_monitor);
            if (theApp.m_main_wnd_data.hide_main_wnd_when_fullscreen)       //Khi đã bật ẩn cửa sổ nổi khi toàn màn hình
            {
                if (m_is_foreground_fullscreen || theApp.m_cfg_data.m_hide_main_window)
                    ShowWindow(SW_HIDE);
                else
                    ShowWindow(SW_RESTORE);
            }
            else            //Nếu không bật ẩn cửa sổ nổi khi toàn màn hình, thì khi có chương trình chuyển sang toàn màn hình sẽ ghim cửa sổ trên cùng một lần
            {
                static bool last_foreground_fullscreen;
                if (!last_foreground_fullscreen && m_is_foreground_fullscreen)
                {
                    SetAlwaysOnTop();
                }
                last_foreground_fullscreen = m_is_foreground_fullscreen;
            }
        }

        if (!m_menu_popuped)
        {
            //Vài giây sau khi chương trình khởi động, thực hiện lại các thao tác “Luôn trên cùng”, “Cho chuột xuyên qua” và “Ẩn cửa sổ chính” theo thiết lập, tránh trường hợp thiết lập không có hiệu lực
            if (m_timer_cnt == 5 || m_timer_cnt == 9)
            {
                if (!theApp.m_cfg_data.m_hide_main_window)
                {
                    SetAlwaysOnTop();
                    SetMousePenetrate();
                }
                else
                {
                    ShowWindow(SW_HIDE);
                }
            }

            if (m_timer_cnt % 300 == 299 && CBaseDialog::IsAllDialogClosed() && !theApp.m_cfg_data.m_hide_main_window && theApp.m_main_wnd_data.m_always_on_top)
            {
                SetAlwaysOnTop();       //Cứ 5 phút ghim cửa sổ trên cùng một lần
            }
        }
        else
        {
            m_tool_tips.Pop();          //Khi đang hiện menu chuột phải thì không hiển thị chú giải chuột
        }

        if (m_timer_cnt % 30 == 26)     //Cứ 30 giây kiểm tra vị trí cửa sổ một lần, khi vị trí cửa sổ thay đổi thì lưu thiết lập
        {
            static int last_pos_x{ -1 }, last_pos_y{ -1 };
            if (last_pos_x != theApp.m_cfg_data.m_position_x || last_pos_y != theApp.m_cfg_data.m_position_y)
            {
                theApp.SaveConfig();
                last_pos_x = theApp.m_cfg_data.m_position_x;
                last_pos_y = theApp.m_cfg_data.m_position_y;
            }
        }

        if (m_timer_cnt % 2 == 1)       //Cứ 2 giây lấy vùng màn hình một lần
        {
            GetScreenSize();
            CheckWindowPos();
        }

        //Chỉ thực hiện xử lý dưới đây khi cửa sổ chính hoặc cửa sổ thanh tác vụ có ít nhất một cửa sổ đang hiển thị
        if (!theApp.m_cfg_data.m_hide_main_window || theApp.m_cfg_data.m_show_task_bar_wnd)
        {
            //Cứ 10 giây kiểm tra một lần xem có thể nhúng vào thanh tác vụ không
            if (!theApp.m_win_version.IsWine() && IsTaskbarWndValid() && m_timer_cnt % 10 == 1)
            {
                if (m_tBarDlg->GetCannotInsertToTaskBar() && m_insert_to_taskbar_cnt < MAX_INSERT_TO_TASKBAR_CNT)
                {
                    CloseTaskBarWnd();
                    OpenTaskBarWnd();
                    m_insert_to_taskbar_cnt++;
                    if (m_tBarDlg->GetCannotInsertToTaskBar() && m_insert_to_taskbar_cnt >= WARN_INSERT_TO_TASKBAR_CNT)
                    {
                        //Ghi nhật ký lỗi
                        CString info = CCommon::LoadText(IDS_CONNOT_INSERT_TO_TASKBAR_ERROR_LOG);
                        info.Replace(_T("<%cnt%>"), CCommon::IntToString(m_insert_to_taskbar_cnt));
                        info.Replace(_T("<%error_code%>"), CCommon::IntToString(m_tBarDlg->GetErrorCode()));
                        CCommon::WriteLog(info, theApp.m_log_path.c_str());
                        if (m_cannot_insert_to_task_bar_warning)      //Đảm bảo thông báo chỉ hiện một lần
                        {
                            //Hiện thông tin lỗi
                            m_cannot_insert_to_task_bar_warning = false;
                            MessageBox(CCommon::LoadText(IDS_CONNOT_INSERT_TO_TASKBAR, CCommon::IntToString(m_tBarDlg->GetErrorCode())), NULL, MB_ICONWARNING);
                        }
                    }
                }
                if (!m_tBarDlg->GetCannotInsertToTaskBar())
                {
                    m_insert_to_taskbar_cnt = 0;
                }
            }
        }

        //Kiểm tra có cần hiện chú giải chuột không
        //setting_data: dữ liệu thiết lập thông báo
        //value: giá trị hiện tại
        //last_value: truyền một biến static hoặc biến vẫn tồn tại sau khi lambda này kết thúc, dùng để lưu giá trị lần trước
        //notify_time: truyền một biến static hoặc biến vẫn tồn tại sau khi lambda này kết thúc, dùng để ghi lại thời điểm hiện thông báo lần trước (số lần bộ định thời kích hoạt)
        //tip_str: thông điệp cần thông báo
        auto checkNotifyTip = [&](GeneralSettingData::NotifyTipSettings setting_data, int value, int& last_value, int& notify_time, LPCTSTR tip_str)
        {
            if (setting_data.enable)
            {
                if (last_value < setting_data.tip_value && value >= setting_data.tip_value && (m_timer_cnt - notify_time > static_cast<unsigned int>(theApp.m_notify_interval)))
                {
                    ShowNotifyTip(CCommon::LoadText(_T("TrafficMonitor "), IDS_NOTIFY, _T("")), tip_str);
                    notify_time = m_timer_cnt;
                }
                last_value = value;
            }
        };

        //Kiểm tra có cần hiện cảnh báo mức sử dụng bộ nhớ vượt ngưỡng không
        CString info;
        info.Format(CCommon::LoadText(IDS_MEMORY_UDAGE_EXCEED, _T(" %d%%!")), theApp.m_memory_usage);
        static int last_memory_usage;
        static int memory_usage_notify_time{ -theApp.m_notify_interval };       //Ghi lại thời điểm hiện thông báo lần trước
        checkNotifyTip(theApp.m_general_data.memory_usage_tip, theApp.m_memory_usage, last_memory_usage, memory_usage_notify_time, info.GetString());

        //Kiểm tra có cần hiện cảnh báo nhiệt độ CPU vượt ngưỡng không
        info.Format(CCommon::LoadText(IDS_CPU_TEMPERATURE_EXCEED, _T(" %d°C!")), static_cast<int>(theApp.m_cpu_temperature));
        static int last_cpu_temp;
        static int cpu_temp_notify_time{ -theApp.m_notify_interval };       //Ghi lại thời điểm hiện thông báo lần trước
        checkNotifyTip(theApp.m_general_data.cpu_temp_tip, static_cast<int>(theApp.m_cpu_temperature), last_cpu_temp, cpu_temp_notify_time, info.GetString());

        //Kiểm tra có cần hiện cảnh báo nhiệt độ GPU vượt ngưỡng không
        info.Format(CCommon::LoadText(IDS_GPU_TEMPERATURE_EXCEED, _T(" %d°C!")), static_cast<int>(theApp.m_gpu_temperature));
        static int last_gpu_temp;
        static int gpu_temp_notify_time{ -theApp.m_notify_interval };       //Ghi lại thời điểm hiện thông báo lần trước
        checkNotifyTip(theApp.m_general_data.gpu_temp_tip, static_cast<int>(theApp.m_gpu_temperature), last_gpu_temp, gpu_temp_notify_time, info.GetString());

        //Kiểm tra có cần hiện cảnh báo nhiệt độ ổ cứng vượt ngưỡng không
        info.Format(CCommon::LoadText(IDS_HDD_TEMPERATURE_EXCEED, _T(" %d°C!")), static_cast<int>(theApp.m_hdd_temperature));
        static int last_hdd_temp;
        static int hdd_temp_notify_time{ -theApp.m_notify_interval };       //Ghi lại thời điểm hiện thông báo lần trước
        checkNotifyTip(theApp.m_general_data.hdd_temp_tip, static_cast<int>(theApp.m_hdd_temperature), last_hdd_temp, hdd_temp_notify_time, info.GetString());

        //Kiểm tra có cần hiện cảnh báo nhiệt độ bo mạch chủ vượt ngưỡng không
        info.Format(CCommon::LoadText(IDS_MBD_TEMPERATURE_EXCEED, _T(" %d°C!")), static_cast<int>(theApp.m_main_board_temperature));
        static int last_main_board_temp;
        static int main_board_temp_notify_time{ -theApp.m_notify_interval };        //Ghi lại thời điểm hiện thông báo lần trước
        checkNotifyTip(theApp.m_general_data.mainboard_temp_tip, static_cast<int>(theApp.m_main_board_temperature), last_main_board_temp, main_board_temp_notify_time, info.GetString());


        //Kiểm tra có cần hiện cảnh báo lưu lượng vượt ngưỡng không
        if (theApp.m_general_data.traffic_tip_enable)
        {
            static __int64 last_today_traffic;
            __int64 traffic_tip_value;
            if (theApp.m_general_data.traffic_tip_unit == 0)
                traffic_tip_value = static_cast<__int64>(theApp.m_general_data.traffic_tip_value) * 1024 * 1024;
            else
                traffic_tip_value = static_cast<__int64>(theApp.m_general_data.traffic_tip_value) * 1024 * 1024 * 1024;

            __int64 today_traffic = theApp.m_today_up_traffic + theApp.m_today_down_traffic;
            if (last_today_traffic < traffic_tip_value && today_traffic >= traffic_tip_value)
            {
                CString info = CCommon::LoadText(IDS_TODAY_TRAFFIC_EXCEED, CCommon::DataSizeToString(today_traffic));
                ShowNotifyTip(CCommon::LoadText(_T("TrafficMonitor "), IDS_NOTIFY, _T("")), info.GetString());
            }
            last_today_traffic = today_traffic;
        }

        CWindowsSettingHelper::CheckWindows10LightTheme();        //Cứ 1 giây kiểm tra một lần xem hệ thống hiện có dùng chủ đề sáng không

        //Tự động đổi màu thanh tác vụ theo chế độ màu Win10 hiện tại
        bool light_mode = CWindowsSettingHelper::IsWindows10LightTheme();
        if (theApp.m_last_light_mode != light_mode)
        {
            theApp.m_last_light_mode = light_mode;
            bool restart_taskbar_dlg{ false };
            if (theApp.m_taskbar_data.auto_adapt_light_theme)
            {
                int style_index = CWindowsSettingHelper::IsWindows10LightTheme() ? theApp.m_taskbar_data.light_default_style : theApp.m_taskbar_data.dark_default_style;
                theApp.m_taskbar_default_style.ApplyDefaultStyle(style_index, theApp.m_taskbar_data);
                theApp.SaveConfig();
                restart_taskbar_dlg = true;
            }
            bool is_taskbar_transparent{ theApp.m_taskbar_data.IsTaskbarTransparent()};
            if (!is_taskbar_transparent)
            {
                theApp.m_taskbar_data.SetTaskabrTransparent(false);
                restart_taskbar_dlg = true;
            }
            if (restart_taskbar_dlg && IsTaskbarWndValid())
            {
                m_tBarDlg->ApplyWindowTransparentColor();
                //CloseTaskBarWnd();
                //OpenTaskBarWnd();

                //Ghi nhật ký gỡ lỗi
                if (theApp.m_debug_log)
                {
                    CString log_str;
                    log_str += _T("Phát hiện thay đổi chế độ sáng/tối của Windows 10.\n");
                    log_str += _T("IsWindows10LightTheme: ");
                    log_str += std::to_wstring(light_mode).c_str();
                    log_str += _T("\n");
                    log_str += _T("auto_adapt_light_theme: ");
                    log_str += std::to_wstring(theApp.m_taskbar_data.auto_adapt_light_theme).c_str();
                    log_str += _T("\n");
                    log_str += _T("is_taskbar_transparent: ");
                    log_str += std::to_wstring(is_taskbar_transparent).c_str();
                    log_str += _T("\n");
                    log_str += _T("taskbar_back_color: ");
                    log_str += std::to_wstring(theApp.m_taskbar_data.back_color).c_str();
                    log_str += _T("\n");
                    log_str += _T("taskbar_transparent_color: ");
                    log_str += std::to_wstring(theApp.m_taskbar_data.transparent_color).c_str();
                    log_str += _T("\n");
                    log_str += _T("taskbar_text_colors: ");
                    for (const auto& item : theApp.m_taskbar_data.text_colors)
                    {
                        log_str += std::to_wstring(item.second.label).c_str();
                        log_str += _T('|');
                        log_str += std::to_wstring(item.second.value).c_str();
                        log_str += _T(", ");
                    }
                    log_str += _T("\n");
                    CCommon::WriteLog(log_str, (theApp.m_config_dir + L".\\debug.log").c_str());
                }
            }

            //Tự động đổi biểu tượng vùng thông báo theo chế độ màu Win10 hiện tại
            if (theApp.m_cfg_data.m_notify_icon_auto_adapt)
            {
                int notify_icon_selected = theApp.m_cfg_data.m_notify_icon_selected;
                theApp.AutoSelectNotifyIcon();
                if (notify_icon_selected != theApp.m_cfg_data.m_notify_icon_selected)
                {
                    m_ntIcon.hIcon = theApp.m_notify_icons[theApp.m_cfg_data.m_notify_icon_selected];
                    if (theApp.m_general_data.show_notify_icon)
                    {
                        DeleteNotifyIcon();
                        AddNotifyIcon();
                    }
                }
            }

            //Tự động đổi skin theo chế độ sáng/tối
            if (theApp.m_win_version.IsWindows10OrLater() && theApp.m_cfg_data.skin_auto_adapt)
            {
                int skin_index = CSkinManager::Instance().FindSkinIndex(light_mode ? theApp.m_cfg_data.skin_name_light_mode : theApp.m_cfg_data.skin_name_dark_mode);
                ApplySkin(skin_index);
            }
        }

        //Tự động đặt màu nền cửa sổ thanh tác vụ theo màu thanh tác vụ
        if (theApp.m_taskbar_data.auto_set_background_color && theApp.m_win_version.IsWindows8OrLater()
            && IsTaskbarWndValid() && theApp.m_taskbar_data.transparent_color != 0
            && !m_is_foreground_fullscreen && theApp.m_taskbar_data.disable_d2d)
        {
            CRect rect;
            ::GetWindowRect(m_tBarDlg->GetSafeHwnd(), rect);
            int pointx{ rect.left - 1 };
            if (theApp.m_taskbar_data.tbar_wnd_on_left && m_tBarDlg->IsTasksbarOnTopOrBottom())
                pointx = rect.right + 1;
            int pointy = rect.bottom;
            if (pointx < 0) pointx = 0;
            if (pointx >= m_screen_size.cx) pointx = m_screen_size.cx - 1;
            if (pointy < 0) pointy = 0;
            if (pointy >= m_screen_size.cy) pointy = m_screen_size.cy - 1;
            COLORREF color = ::GetPixel(m_desktop_dc, pointx, pointy);        //Lấy màu tại vị trí 1 pixel bên trái cửa sổ thanh tác vụ làm màu nền
            if (!CCommon::IsColorSimilar(color, theApp.m_taskbar_data.back_color) && (/*CWindowsSettingHelper::IsWindows10LightTheme() ||*/ color != 0))
            {
                bool is_taskbar_transparent{ theApp.m_taskbar_data.IsTaskbarTransparent()};
                theApp.m_taskbar_data.back_color = color;
                theApp.m_taskbar_data.SetTaskabrTransparent(is_taskbar_transparent);
                if (is_taskbar_transparent)
                    m_tBarDlg->ApplyWindowTransparentColor();
            }
        }

        //Ghi nhật ký lỗi khi phát hiện màu nền và màu chữ đều là màu đen
        static bool erro_log_write{ false };
        if (theApp.m_taskbar_data.back_color == 0 && !theApp.m_taskbar_data.text_colors.empty() && theApp.m_taskbar_data.text_colors.begin()->second.label == 0)
        {
            if (!erro_log_write)
            {
                CString log_str;
                log_str.Format(_T("Phát hiện màu nền và màu chữ đều là màu đen. IsWindows10LightTheme: %d, thời điểm khởi động hệ thống: %d/%.2d/%.2d %.2d:%.2d:%.2d"),
                    light_mode, m_start_time.wYear, m_start_time.wMonth, m_start_time.wDay, m_start_time.wHour, m_start_time.wMinute, m_start_time.wSecond);
                CCommon::WriteLog(log_str, theApp.m_log_path.c_str());
                erro_log_write = true;
            }
        }
        else
        {
            erro_log_write = false;
        }

        UpdateNotifyIconTip();


    }

    if (nIDEvent == DELAY_TIMER)
    {
        AutoSelect();
        KillTimer(DELAY_TIMER);
    }

    if (nIDEvent == TASKBAR_TIMER)
    {
        ++m_taskbar_timer_cnt;
        if (m_taskbar_timer_cnt % 5 == 0 && theApp.m_cfg_data.m_show_task_bar_wnd && theApp.m_taskbar_data.show_taskbar_wnd_in_secondary_display)
        {
            static int last_taskbar_num = 0;
            int taskbar_num = CTaskbarHelper::GetSecondaryTaskbarNum();
            //Nếu số thanh tác vụ trên màn hình phụ thay đổi thì khởi động lại cửa sổ thanh tác vụ
            if (last_taskbar_num != taskbar_num)
            {
                last_taskbar_num = taskbar_num;
                //Khởi động lại cửa sổ thanh tác vụ sau một khoảng thời gian trễ
                KillTimer(RESTART_TASKBAR_TIMER);
                SetTimer(RESTART_TASKBAR_TIMER, 500, [](HWND, UINT, UINT_PTR, DWORD) {
                    theApp.m_pMainWnd->SendMessage(WM_REOPEN_TASKBAR_WND);
                    ::KillTimer(theApp.m_pMainWnd->GetSafeHwnd(), RESTART_TASKBAR_TIMER);
                });
            }
        }

        if (IsTaskbarWndValid())
        {
            //Khi ẩn cửa sổ chính ngay từ lúc khởi động thì không nhận được thông điệp dpichange, nên cần kiểm tra thủ công
            //Cứ mỗi 100ms*10 kiểm tra DPI màn hình một lần, và kiểm tra phiên bản hệ điều hành ít nhất có thể
            if (m_taskbar_timer_cnt % 10 == 0 && theApp.m_win_version.IsWindows8Point1OrLater())
            {
                CTaskBarDlg::CheckWindowMonitorDPIAndHandle(*m_tBarDlg, [p_TaskBarDlg = m_tBarDlg](UINT new_dpi_x, UINT new_dpi_y)
                                                            {
                                                                // auto s_dpi = std::to_string(new_dpi_x);
                                                                // s_dpi += '\n';
                                                                // TRACE(s_dpi.c_str());
                                                                //Xét đến trường hợp cửa sổ thanh tác vụ và cửa sổ chính không ở cùng một màn hình, DPI có thể khác nhau
                                                                //Thiết lập DPI và làm mới cửa sổ
                                                                p_TaskBarDlg->SetDPI(new_dpi_x);
                                                                p_TaskBarDlg->SetTextFont();
                                                                p_TaskBarDlg->CalculateWindowSize(); });
            }

            m_tBarDlg->AdjustWindowPos();
            m_tBarDlg->Invalidate(FALSE);
        }
    }

    if (nIDEvent == DELETE_NOTIFY_ICON_TIMER)
    {
        DeleteNotifyIcon();
        KillTimer(DELETE_NOTIFY_ICON_TIMER);
    }

    CDialog::OnTimer(nIDEvent);
}


void CTrafficMonitorDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
    // TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây
    CheckClickedItem(point);
    bool is_plugin_item_clicked = (m_clicked_item.IsPlugin() && m_clicked_item.PluginItem() != nullptr);
    ITMPlugin* plugin{};
    if (is_plugin_item_clicked)
    {
        plugin = theApp.m_plugins.GetPluginByItem(m_clicked_item.PluginItem());
        if (plugin != nullptr && plugin->GetAPIVersion() >= 3)
        {
            if (m_clicked_item.PluginItem()->OnMouseEvent(IPluginItem::MT_RCLICKED, point.x, point.y, (void*)GetSafeHwnd(), 0) != 0)
                return;
        }
    }
    //Thiết lập nhấp chuột phải thì hiện menu
    CMenu* pContextMenu = (is_plugin_item_clicked ? theApp.m_main_menu_plugin.GetSubMenu(0) : theApp.m_main_menu.GetSubMenu(0));
    CPoint point1;  //Khai báo vị trí dùng để xác định vị trí con trỏ chuột
    GetCursorPos(&point1);  //Lấy vị trí con trỏ chuột hiện tại để menu đi theo con trỏ
    //Thiết lập mục menu mặc định
    switch (theApp.m_main_wnd_data.double_click_action)
    {
    case DoubleClickAction::CONNECTION_INFO:
        pContextMenu->SetDefaultItem(ID_NETWORK_INFO);
        break;
        //case DoubleClickAction::HISTORY_TRAFFIC:
        //  pContextMenu->SetDefaultItem(ID_TRAFFIC_HISTORY);
        //  break;
    case DoubleClickAction::SHOW_MORE_INFO:
        pContextMenu->SetDefaultItem(ID_SHOW_CPU_MEMORY);
        break;
    case DoubleClickAction::OPTIONS:
        pContextMenu->SetDefaultItem(ID_OPTIONS);
        break;
        //case DoubleClickAction::CHANGE_SKIN:
        //  pContextMenu->SetDefaultItem(ID_CHANGE_SKIN);
        //  break;
    default:
        pContextMenu->SetDefaultItem(-1);
        break;
    }

    if (plugin != nullptr)
    {
        //Đổi văn bản hiển thị của menu plugin trong menu chuột phải thành tên plugin
        pContextMenu->ModifyMenu(17, MF_BYPOSITION, 17, plugin->GetInfo(ITMPlugin::TMI_NAME));
        //Lấy biểu tượng plugin
        HICON plugin_icon{};
        if (plugin->GetAPIVersion() >= 5)
            plugin_icon = (HICON)plugin->GetPluginIcon();
        //Thiết lập biểu tượng plugin
        if (plugin_icon != nullptr)
            CMenuIcon::AddIconToMenuItem(pContextMenu->GetSafeHmenu(), 17, TRUE, plugin_icon);
    }
    //Cập nhật menu con của plugin
    theApp.UpdatePluginMenu(&theApp.m_main_menu_plugin_sub_menu, plugin, 2);

    pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this); //Hiển thị menu bật lên tại vị trí chỉ định

    CDialog::OnRButtonUp(nFlags, point1);
}


void CTrafficMonitorDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây
    CheckClickedItem(point);
    bool plugin_item_clicked = false;   //Đã phản hồi sự kiện nhấp chuột trái vào mục plugin hay chưa
    if (m_clicked_item.IsPlugin() && m_clicked_item.PluginItem() != nullptr)      //Mục được nhấp có phải mục plugin không
    {
        ITMPlugin* plugin = theApp.m_plugins.GetPluginByItem(m_clicked_item.PluginItem());
        if (plugin != nullptr && plugin->GetAPIVersion() >= 3)
        {
            if (m_clicked_item.PluginItem()->OnMouseEvent(IPluginItem::MT_LCLICKED, point.x, point.y, (void*)GetSafeHwnd(), 0) != 0)
            {
                plugin_item_clicked = true;
                Invalidate();
            }
        }
    }

    //Khi chưa khóa vị trí cửa sổ thì cho phép kéo cửa sổ bằng cách nhấp vào bên trong cửa sổ
    if (!theApp.m_main_wnd_data.m_lock_window_pos && !plugin_item_clicked)
        PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
    CDialog::OnLButtonDown(nFlags, point);
}


void CTrafficMonitorDlg::OnNetworkInfo()
{
    // TODO: Thêm mã xử lý lệnh tại đây
    //Mở hộp thoại “Chi tiết kết nối”
    //Truyền bản sao dữ liệu cho hộp thoại: luồng giám sát có thể yêu cầu cấp phát lại m_pIfTable/m_connections khi hộp thoại đang mở
    vector<NetWorkConection> connections;
    vector<MIB_IFROW> if_rows;
    {
        CSingleLock lock(&m_connections_lock, TRUE);
        connections = m_connections;
        if (m_pIfTable != nullptr)
            if_rows.assign(m_pIfTable->table, m_pIfTable->table + m_pIfTable->dwNumEntries);
    }
    CNetworkInfoDlg aDlg(connections, if_rows, m_connection_selected);
    ////Truyền số byte đã gửi và nhận kể từ khi khởi động cho lớp CNetworkInfoDlg
    //aDlg.m_in_bytes = m_pIfTable->table[m_connections[m_connection_selected].index].dwInOctets - m_connections[m_connection_selected].in_bytes;
    //aDlg.m_out_bytes = m_pIfTable->table[m_connections[m_connection_selected].index].dwOutOctets - m_connections[m_connection_selected].out_bytes;
    aDlg.m_start_time = m_start_time;
    aDlg.DoModal();
    //SetAlwaysOnTop(); //Vì trong hộp thoại “Chi tiết kết nối” đã bỏ ghim cửa sổ trên cùng, nên sau khi đóng hộp thoại cần đặt lại cửa sổ trên cùng
    if (m_tBarDlg != nullptr)
        m_tBarDlg->m_tool_tips.SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);  //Đặt lại chú giải của cửa sổ thanh tác vụ luôn trên cùng
}


void CTrafficMonitorDlg::OnAlwaysOnTop()
{
    // TODO: Thêm mã xử lý lệnh tại đây
    theApp.m_main_wnd_data.m_always_on_top = !theApp.m_main_wnd_data.m_always_on_top;
    SetAlwaysOnTop();
    theApp.SaveConfig();
}


void CTrafficMonitorDlg::OnTransparency100()
{
    // TODO: Thêm mã xử lý lệnh tại đây
    theApp.m_cfg_data.m_transparency = 100;
    SetTransparency();
    theApp.SaveConfig();
}


void CTrafficMonitorDlg::OnTransparency80()
{
    // TODO: Thêm mã xử lý lệnh tại đây
    theApp.m_cfg_data.m_transparency = 80;
    SetTransparency();
    theApp.SaveConfig();
}


void CTrafficMonitorDlg::OnTransparency60()
{
    // TODO: Thêm mã xử lý lệnh tại đây
    theApp.m_cfg_data.m_transparency = 60;
    SetTransparency();
    theApp.SaveConfig();
}


void CTrafficMonitorDlg::OnTransparency40()
{
    // TODO: Thêm mã xử lý lệnh tại đây
    theApp.m_cfg_data.m_transparency = 40;
    SetTransparency();
    theApp.SaveConfig();
}


void CTrafficMonitorDlg::OnClose()
{
    // TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây
    theApp.m_cannot_save_config_warning = true;
    theApp.m_cannot_save_global_config_warning = true;
    theApp.SaveConfig();    //Lưu thiết lập vào tệp ini trước khi thoát
    theApp.SaveGlobalConfig();
    SaveHistoryTrafficFull();  // Khi thoát thì dùng lưu đầy đủ, đảm bảo mọi dữ liệu đều được lưu
    BackupHistoryTrafficFile();

    if (IsTaskbarWndValid())
        m_tBarDlg->OnCancel();

    //Đảm bảo đóng mọi cửa sổ trước khi thoát
    for (const auto& item : CBaseDialog::AllUniqueHandels())
    {
        ::SendMessage(item.second, WM_COMMAND, IDCANCEL, 0);
    }

    CDialog::OnClose();
}


BOOL CTrafficMonitorDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // TODO: Thêm mã chuyên biệt và/hoặc gọi lớp cơ sở tại đây
    UINT uMsg = LOWORD(wParam);
    if (uMsg == ID_SELECT_ALL_CONNECTION)
    {
        theApp.m_cfg_data.m_select_all = true;
        theApp.m_cfg_data.m_auto_select = false;
        m_connection_change_flag = true;
    }
    //Xử lý khi chọn một mục trong menu con “Chọn kết nối mạng”
    if (uMsg == ID_SELETE_CONNECTION)   //Đã chọn mục menu “Tự động chọn”
    {
        AutoSelect();
        theApp.m_cfg_data.m_auto_select = true;
        theApp.m_cfg_data.m_select_all = false;
        theApp.SaveConfig();
        m_connection_change_flag = true;
    }
    if (uMsg > ID_SELECT_ALL_CONNECTION && uMsg <= ID_SELECT_ALL_CONNECTION + m_connections.size()) //Đã chọn một kết nối mạng
    {
        m_connection_selected = uMsg - ID_SELECT_ALL_CONNECTION - 1;
        theApp.m_cfg_data.m_connection_name = GetConnection(m_connection_selected).description_2;
        m_connection_name_preferd = theApp.m_cfg_data.m_connection_name;
        theApp.m_cfg_data.m_auto_select = false;
        theApp.m_cfg_data.m_select_all = false;
        theApp.SaveConfig();
        m_connection_change_flag = true;
    }
#ifdef DEBUG
    if (uMsg == ID_CMD_TEST)
    {
        CTest::TestCommand();
        //ShowNotifyTip(CCommon::LoadText(_T("TrafficMonitor "), IDS_NOTIFY, _T("")), _T("Thông báo thử nghiệm"));

    }
#endif // DEBUG
    //Đã chọn lệnh của plugin
    if (uMsg >= ID_PLUGIN_COMMAND_START && uMsg <= ID_PLUGIN_COMMAND_MAX)
    {
        int index = uMsg - ID_PLUGIN_COMMAND_START;
        if (m_clicked_item.IsPlugin() && m_clicked_item.PluginItem() != nullptr)
        {
            ITMPlugin* plugin = theApp.m_plugins.GetPluginByItem(m_clicked_item.PluginItem());
            if (plugin != nullptr && plugin->GetAPIVersion() >= 5)
            {
                plugin->OnPluginCommand(index, (void*)GetSafeHwnd(), nullptr);
            }
        }
    }


    return CDialog::OnCommand(wParam, lParam);
    }


void CTrafficMonitorDlg::OnInitMenu(CMenu* pMenu)
{
    CDialog::OnInitMenu(pMenu);

    // TODO: Thêm mã xử lý thông điệp tại đây
    m_menu_popuped = true;

    pMenu->CheckMenuItem(ID_ALWAYS_ON_TOP, MF_BYCOMMAND | (theApp.m_main_wnd_data.m_always_on_top ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_LOCK_WINDOW_POS, MF_BYCOMMAND | (theApp.m_main_wnd_data.m_lock_window_pos ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_SHOW_CPU_MEMORY, MF_BYCOMMAND | (theApp.m_cfg_data.m_show_more_info ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_MOUSE_PENETRATE, MF_BYCOMMAND | (theApp.m_main_wnd_data.m_mouse_penetrate ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_SHOW_TASK_BAR_WND, MF_BYCOMMAND | (theApp.m_cfg_data.m_show_task_bar_wnd ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_SHOW_MAIN_WND, MF_BYCOMMAND | (!theApp.m_cfg_data.m_hide_main_window ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_ALOW_OUT_OF_BORDER, MF_BYCOMMAND | (theApp.m_main_wnd_data.m_alow_out_of_border ? MF_CHECKED : MF_UNCHECKED));

    //Thiết lập trạng thái chọn của các tùy chọn trong menu con “Chọn kết nối”
    SetConnectionMenuState(theApp.m_main_menu.GetSubMenu(0)->GetSubMenu(0));
    SetConnectionMenuState(theApp.m_main_menu_plugin.GetSubMenu(0)->GetSubMenu(0));

    //Thiết lập trạng thái chọn của các tùy chọn trong menu con “Độ mờ đục cửa sổ”
    switch (theApp.m_cfg_data.m_transparency)
    {
    case 100: pMenu->CheckMenuRadioItem(ID_TRANSPARENCY_100, ID_TRANSPARENCY_40, ID_TRANSPARENCY_100, MF_BYCOMMAND | MF_CHECKED); break;
    case 80: pMenu->CheckMenuRadioItem(ID_TRANSPARENCY_100, ID_TRANSPARENCY_40, ID_TRANSPARENCY_80, MF_BYCOMMAND | MF_CHECKED); break;
    case 60: pMenu->CheckMenuRadioItem(ID_TRANSPARENCY_100, ID_TRANSPARENCY_40, ID_TRANSPARENCY_60, MF_BYCOMMAND | MF_CHECKED); break;
    case 40: pMenu->CheckMenuRadioItem(ID_TRANSPARENCY_100, ID_TRANSPARENCY_40, ID_TRANSPARENCY_40, MF_BYCOMMAND | MF_CHECKED); break;
    default: break;
    }

    if (theApp.IsForceShowNotifyIcon())    //Nếu không hiển thị cửa sổ thanh tác vụ, đồng thời đã ẩn cửa sổ chính hoặc bật cho chuột xuyên qua, thì vô hiệu hóa mục menu “Hiển thị biểu tượng vùng thông báo”
        pMenu->EnableMenuItem(ID_SHOW_NOTIFY_ICON, MF_BYCOMMAND | MF_GRAYED);
    else
        pMenu->EnableMenuItem(ID_SHOW_NOTIFY_ICON, MF_BYCOMMAND | MF_ENABLED);

    pMenu->EnableMenuItem(ID_SELECT_ALL_CONNECTION, MF_BYCOMMAND | (theApp.m_general_data.show_all_interface ? MF_GRAYED : MF_ENABLED));

    pMenu->EnableMenuItem(ID_CHECK_UPDATE, MF_BYCOMMAND | (theApp.IsCheckingForUpdate() ? MF_GRAYED : MF_ENABLED));

    //Thiết lập trạng thái đánh dấu cho lệnh của plugin
    ITMPlugin* plugin = theApp.m_plugins.GetPluginByItem(m_clicked_item.PluginItem());
    if (plugin != nullptr && plugin->GetAPIVersion() >= 5)
    {
        for (int i = ID_PLUGIN_COMMAND_START; i <= ID_PLUGIN_COMMAND_MAX; i++)
        {
            bool checked = (plugin->IsCommandChecked(i) != 0);
            pMenu->CheckMenuItem(i, MF_BYCOMMAND | (checked ? MF_CHECKED : MF_UNCHECKED));
        }
    }

    //pMenu->SetDefaultItem(ID_NETWORK_INFO);
}


BOOL CTrafficMonitorDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Thêm mã chuyên biệt và/hoặc gọi lớp cơ sở tại đây
    //Chặn thoát bằng phím Enter và ESC
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;

    if (theApp.m_main_wnd_data.show_tool_tip && m_tool_tips.GetSafeHwnd())
    {
        m_tool_tips.RelayEvent(pMsg);
    }

    if (pMsg->message == WM_KEYDOWN)
    {
        bool ctrl = (GetKeyState(VK_CONTROL) & 0x80);
        bool shift = (GetKeyState(VK_SHIFT) & 0x8000);
        bool alt = (GetKeyState(VK_MENU) & 0x8000);
        ITMPlugin* plugin = theApp.m_plugins.GetPluginByItem(m_clicked_item.PluginItem());
        if (plugin != nullptr && plugin->GetAPIVersion() >= 4)
        {
            if (m_clicked_item.PluginItem()->OnKeboardEvent(static_cast<int>(pMsg->wParam), ctrl, shift, alt, (void*)GetSafeHwnd(), IPluginItem::KF_TASKBAR_WND) != 0)
                return TRUE;
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}


void CTrafficMonitorDlg::OnLockWindowPos()
{
    // TODO: Thêm mã xử lý lệnh tại đây
    theApp.m_main_wnd_data.m_lock_window_pos = !theApp.m_main_wnd_data.m_lock_window_pos;
    theApp.SaveConfig();
}


void CTrafficMonitorDlg::OnMove(int x, int y)
{
    CDialog::OnMove(x, y);

    // TODO: Thêm mã xử lý thông điệp tại đây

    if (!m_first_start)
    {
        theApp.m_cfg_data.m_position_x = x;
        theApp.m_cfg_data.m_position_y = y;
    }

    ////Đảm bảo cửa sổ không vượt ra ngoài phạm vi màn hình
    //CheckWindowPos();
}



afx_msg LRESULT CTrafficMonitorDlg::OnNotifyIcon(WPARAM wParam, LPARAM lParam)
{
    bool dialog_exist{ false };
    HWND handle{};
    if (lParam == WM_LBUTTONDOWN || lParam == WM_RBUTTONUP || lParam == WM_LBUTTONDBLCLK)
    {
        for (const auto& item : CBaseDialog::AllUniqueHandels())
        {
            if (IsWindow(item.second))
            {
                dialog_exist = true;
                handle = item.second;
                break;
            }
        }
    }

    if (lParam == WM_LBUTTONDOWN)
    {
        if (!theApp.m_cfg_data.m_hide_main_window)
        {
            if (dialog_exist)       //Khi đang mở hộp thoại, nhấp biểu tượng vùng thông báo sẽ đặt tiêu điểm vào hộp thoại
            {
                ::SetForegroundWindow(handle);
            }
            else        //Khi không có hộp thoại nào đang mở thì hiển thị cửa sổ chính
            {
                ShowWindow(SW_RESTORE);
                theApp.m_cfg_data.m_hide_main_window = false;
                SetForegroundWindow();
                SetAlwaysOnTop();
                CheckWindowPos();
                theApp.SaveConfig();
            }
        }
    }
    if (lParam == WM_RBUTTONUP)
    {
        if (dialog_exist)       //Khi đang mở hộp thoại, nhấp biểu tượng vùng thông báo sẽ đặt tiêu điểm vào hộp thoại
        {
            ::SetForegroundWindow(handle);
        }
        else
        {
            //Nhấp chuột phải vào vùng thông báo thì hiện menu chuột phải
            if (IsTaskbarWndValid())        //Nếu đang hiển thị cửa sổ thanh tác vụ thì sau khi nhấp chuột phải vào biểu tượng vùng thông báo sẽ đặt tiêu điểm vào cửa sổ thanh tác vụ
                m_tBarDlg->SetForegroundWindow();
            else                //Ngược lại thì đặt tiêu điểm vào cửa sổ chính
                SetForegroundWindow();
            CPoint point1;  //Khai báo vị trí dùng để xác định vị trí con trỏ chuột
            GetCursorPos(&point1);  //Lấy vị trí con trỏ chuột hiện tại để menu đi theo con trỏ
            theApp.m_main_menu.GetSubMenu(0)->SetDefaultItem(-1);       //Thiết lập không có mục menu mặc định
            theApp.m_main_menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this); //Hiển thị menu bật lên tại vị trí chỉ định

            CheckWindowPos();
        }
    }
    if (lParam == WM_LBUTTONDBLCLK)
    {
        if (dialog_exist)       //Khi đang mở hộp thoại, nhấp biểu tượng vùng thông báo sẽ đặt tiêu điểm vào hộp thoại
        {
            ::SetForegroundWindow(handle);
        }
        else        //Khi không có hộp thoại nào đang mở thì hiển thị cửa sổ chính
        {
            ShowWindow(SW_RESTORE);
            theApp.m_cfg_data.m_hide_main_window = false;
            SetForegroundWindow();
            SetAlwaysOnTop();
            CheckWindowPos();
            theApp.SaveConfig();
        }
    }
    return 0;
}


void CTrafficMonitorDlg::OnShowNotifyIcon()
{
    // TODO: Thêm mã xử lý lệnh tại đây
    if (theApp.m_general_data.show_notify_icon)
    {
        DeleteNotifyIcon();
        theApp.m_general_data.show_notify_icon = false;
    }
    else
    {
        AddNotifyIcon();
        theApp.m_general_data.show_notify_icon = true;
    }
    theApp.SaveConfig();
}


void CTrafficMonitorDlg::OnDestroy()
{
    CDialog::OnDestroy();

    //Xóa biểu tượng vùng thông báo khi chương trình thoát
    ::Shell_NotifyIcon(NIM_DELETE, &m_ntIcon);

    // Dừng luồng giám sát
    ExitMonitorThread();
}


void CTrafficMonitorDlg::OnShowCpuMemory()
{
    // TODO: Thêm mã xử lý lệnh tại đây
    CRect rect;
    GetWindowRect(rect);
    theApp.m_cfg_data.m_show_more_info = !theApp.m_cfg_data.m_show_more_info;
    if (theApp.m_cfg_data.m_show_more_info)
    {
        rect.right = rect.left + m_skin.GetLayoutInfo().layout_l.width;
        rect.bottom = rect.top + m_skin.GetLayoutInfo().layout_l.width;
        MoveWindow(rect);
        CheckWindowPos();
    }
    else
    {
        rect.right = rect.left + m_skin.GetLayoutInfo().layout_s.width;
        rect.bottom = rect.top + m_skin.GetLayoutInfo().layout_s.height;
        MoveWindow(rect);
        CheckWindowPos();
    }
    LoadBackGroundImage();
    SetItemPosition();
    Invalidate(FALSE);
    theApp.SaveConfig();
}


//Xử lý khi cửa sổ thanh tác vụ bật/tắt hiển thị mức sử dụng CPU và bộ nhớ
void CTrafficMonitorDlg::OnShowCpuMemory2()
{
    // TODO: Thêm mã xử lý lệnh tại đây
    if (IsTaskbarWndValid())
    {
        bool show_cpu_memory = (theApp.m_taskbar_data.display_item.Contains(TDI_CPU) || theApp.m_taskbar_data.display_item.Contains(TDI_MEMORY));
        if (show_cpu_memory)
        {
            theApp.m_taskbar_data.display_item.Remove(TDI_CPU);
            theApp.m_taskbar_data.display_item.Remove(TDI_MEMORY);
        }
        else
        {
            theApp.m_taskbar_data.display_item.Add(TDI_CPU);
            theApp.m_taskbar_data.display_item.Add(TDI_MEMORY);
        }
        //theApp.m_cfg_data.m_tbar_show_cpu_memory = !theApp.m_cfg_data.m_tbar_show_cpu_memory;
        //Khi bật/tắt hiển thị mức sử dụng CPU và bộ nhớ thì xóa cửa sổ thanh tác vụ rồi hiển thị lại
        //CloseTaskBarWnd();
        //OpenTaskBarWnd();
        m_tBarDlg->WidthChanged();
    }
}


void CTrafficMonitorDlg::OnMousePenetrate()
{
    // TODO: Thêm mã xử lý lệnh tại đây
    theApp.m_main_wnd_data.m_mouse_penetrate = !theApp.m_main_wnd_data.m_mouse_penetrate;
    SetMousePenetrate();
    if (!theApp.m_general_data.show_notify_icon && theApp.IsForceShowNotifyIcon())   //Khi cho chuột xuyên qua, nếu biểu tượng thông báo chưa hiển thị thì hiển thị nó, nếu không sẽ không mở được menu chuột phải
    {
        //Thêm biểu tượng vùng thông báo
        AddNotifyIcon();
        theApp.m_general_data.show_notify_icon = true;
    }

    //Sau khi bật cho chuột xuyên qua, hiện thông báo hướng dẫn người dùng cách tắt tính năng này
    if (theApp.m_main_wnd_data.m_mouse_penetrate && theApp.m_show_mouse_panetrate_tip)
    {
        if (MessageBox(CCommon::LoadText(IDS_MOUSE_PENETRATE_TIP_INFO), NULL, MB_ICONINFORMATION | MB_OKCANCEL) == IDCANCEL)        //Nhấn “Hủy” thì không nhắc lại
        {
            theApp.m_show_mouse_panetrate_tip = false;
        }
    }

    theApp.SaveConfig();
}


void CTrafficMonitorDlg::OnShowTaskBarWnd()
{
    // TODO: Thêm mã xử lý lệnh tại đây
    if (m_tBarDlg != nullptr)
    {
        CloseTaskBarWnd();
    }
    if (!theApp.m_cfg_data.m_show_task_bar_wnd)
    {
        theApp.m_cfg_data.m_show_task_bar_wnd = true;
        OpenTaskBarWnd();
    }
    else
    {
        theApp.m_cfg_data.m_show_task_bar_wnd = false;
        //Sau khi đóng cửa sổ thanh tác vụ, nếu không hiển thị biểu tượng vùng thông báo và không hiển thị cửa sổ chính hoặc đã bật cho chuột xuyên qua, thì hiển thị biểu tượng vùng thông báo
        if (!theApp.m_general_data.show_notify_icon && theApp.IsForceShowNotifyIcon())
        {
            AddNotifyIcon();
            theApp.m_general_data.show_notify_icon = true;
        }
    }
    theApp.SaveConfig();
}


void CTrafficMonitorDlg::OnAppAbout()
{
    // TODO: Thêm mã xử lý lệnh tại đây
    //Mở hộp thoại “Giới thiệu”
    CAboutDlg aDlg;
    aDlg.DoModal();
}


//Thông điệp này được kích hoạt khi Windows Explorer khởi động lại
LRESULT CTrafficMonitorDlg::OnTaskBarCreated(WPARAM wParam, LPARAM lParam)
{
    if (m_tBarDlg != nullptr)
    {
        CloseTaskBarWnd();
        if (theApp.m_general_data.show_notify_icon)
        {
            //Thêm lại biểu tượng vùng thông báo
            ::Shell_NotifyIcon(NIM_ADD, &m_ntIcon);
        }
        OpenTaskBarWnd();
    }
    else
    {
        if (theApp.m_general_data.show_notify_icon)
            ::Shell_NotifyIcon(NIM_ADD, &m_ntIcon);
    }
    return LRESULT();
}


void CTrafficMonitorDlg::OnShowMainWnd()
{
    // TODO: Thêm mã xử lý lệnh tại đây
    if (!theApp.m_cfg_data.m_hide_main_window)
    {
        ShowWindow(SW_HIDE);
        theApp.m_cfg_data.m_hide_main_window = true;
        //Sau khi ẩn cửa sổ chính, nếu không hiển thị biểu tượng vùng thông báo thì hiển thị nó
        if (!theApp.m_general_data.show_notify_icon && theApp.IsForceShowNotifyIcon())
        {
            AddNotifyIcon();
            theApp.m_general_data.show_notify_icon = true;
        }
    }
    else
    {
        ShowWindow(SW_RESTORE);
        theApp.m_cfg_data.m_hide_main_window = false;
    }
    theApp.SaveConfig();
}


void CTrafficMonitorDlg::OnChangeSkin()
{
    // TODO: Thêm mã xử lý lệnh tại đây
    CSkinDlg skinDlg;
    //Khởi tạo dữ liệu của đối tượng CSkinDlg
    skinDlg.m_skin_selected = m_skin_selected;
    if (skinDlg.DoModal() == IDOK)
    {
        ApplySkin(skinDlg.m_skin_selected);
    }
}



void CTrafficMonitorDlg::OnTrafficHistory()
{
    // TODO: Thêm mã xử lý lệnh tại đây
    CHistoryTrafficDlg historyDlg(m_history_traffic.GetTraffics());
    historyDlg.DoModal();
}


void CTrafficMonitorDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây

    CDialog::OnMouseMove(nFlags, point);
}


void CTrafficMonitorDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    // TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây
    CheckClickedItem(point);
    if (m_clicked_item.IsPlugin() && m_clicked_item.PluginItem() != nullptr)
    {
        ITMPlugin* plugin = theApp.m_plugins.GetPluginByItem(m_clicked_item.PluginItem());
        if (plugin != nullptr && plugin->GetAPIVersion() >= 3)
        {
            if (m_clicked_item.PluginItem()->OnMouseEvent(IPluginItem::MT_DBCLICKED, point.x, point.y, (void*)GetSafeHwnd(), 0) != 0)
                return;
        }
    }

    switch (theApp.m_main_wnd_data.double_click_action)
    {
    case DoubleClickAction::CONNECTION_INFO:
        OnNetworkInfo();            //Nhấp đúp thì mở hộp thoại “Chi tiết kết nối”
        break;
    case DoubleClickAction::HISTORY_TRAFFIC:
        OnTrafficHistory();         //Nhấp đúp thì mở hộp thoại “Thống kê lưu lượng”
        break;
    case DoubleClickAction::SHOW_MORE_INFO:
        OnShowCpuMemory();          //Bật/tắt hiển thị mức sử dụng CPU và bộ nhớ
        break;
    case DoubleClickAction::OPTIONS:
        OnOptions();                //Nhấp đúp thì mở hộp thoại “Tùy chọn”
        break;
    case DoubleClickAction::TASK_MANAGER:
        ShellExecuteW(NULL, _T("open"), (theApp.m_system_dir + L"\\Taskmgr.exe").c_str(), NULL, NULL, SW_NORMAL);       //Mở Trình quản lý tác vụ (Task Manager)
        break;
    case DoubleClickAction::SEPCIFIC_APP:
        ShellExecuteW(NULL, _T("open"), (theApp.m_main_wnd_data.double_click_exe).c_str(), NULL, NULL, SW_NORMAL);  //Mở chương trình chỉ định, mặc định là Trình quản lý tác vụ
        break;
    case DoubleClickAction::CHANGE_SKIN:
        OnChangeSkin();             //Nhấp đúp thì mở hộp thoại “Đổi skin”
        break;
    default:
        break;
    }
    CDialog::OnLButtonDblClk(nFlags, point);
}


void CTrafficMonitorDlg::OnOptions()
{
    _OnOptions(0, this);
}


//Mở hộp thoại “Tùy chọn” từ menu chuột phải của cửa sổ thanh tác vụ
void CTrafficMonitorDlg::OnOptions2()
{
    CWnd* pParent = this;
    if (IsTaskbarWndValid())
        pParent = m_tBarDlg;
    _OnOptions(1, pParent);
}


afx_msg LRESULT CTrafficMonitorDlg::OnExitmenuloop(WPARAM wParam, LPARAM lParam)
{
    m_menu_popuped = false;
    return 0;
}


void CTrafficMonitorDlg::OnChangeNotifyIcon()
{
    // TODO: Thêm mã xử lý lệnh tại đây
    CIconSelectDlg dlg(theApp.m_cfg_data.m_notify_icon_selected);
    dlg.SetAutoAdaptNotifyIcon(theApp.m_cfg_data.m_notify_icon_auto_adapt);
    if (dlg.DoModal() == IDOK)
    {
        theApp.m_cfg_data.m_notify_icon_selected = dlg.GetIconSelected();
        theApp.m_cfg_data.m_notify_icon_auto_adapt = dlg.AutoAdaptNotifyIcon();
        m_ntIcon.hIcon = theApp.m_notify_icons[theApp.m_cfg_data.m_notify_icon_selected];
        if (theApp.m_cfg_data.m_notify_icon_auto_adapt)
            theApp.AutoSelectNotifyIcon();
        if (theApp.m_general_data.show_notify_icon)
        {
            DeleteNotifyIcon();
            AddNotifyIcon();
        }
        theApp.SaveConfig();
    }
}


void CTrafficMonitorDlg::OnAlowOutOfBorder()
{
    // TODO: Thêm mã xử lý lệnh tại đây
    theApp.m_main_wnd_data.m_alow_out_of_border = !theApp.m_main_wnd_data.m_alow_out_of_border;
    CheckWindowPos();
}


void CTrafficMonitorDlg::OnCheckUpdate()
{
    // TODO: Thêm mã xử lý lệnh tại đây
    theApp.CheckUpdateInThread(true);
}


afx_msg LRESULT CTrafficMonitorDlg::OnTaskbarMenuPopedUp(WPARAM wParam, LPARAM lParam)
{
    //Thiết lập trạng thái chọn của các tùy chọn trong menu con “Chọn kết nối”
    SetConnectionMenuState(theApp.m_taskbar_menu.GetSubMenu(0)->GetSubMenu(0));
    SetConnectionMenuState(theApp.m_taskbar_menu_plugin.GetSubMenu(0)->GetSubMenu(0));
    return 0;
}


//Xử lý khi cửa sổ thanh tác vụ bật/tắt hiển thị tốc độ mạng
void CTrafficMonitorDlg::OnShowNetSpeed()
{
    // TODO: Thêm mã xử lý lệnh tại đây
    if (IsTaskbarWndValid())
    {
        bool show_net_speed = (theApp.m_taskbar_data.display_item.Contains(TDI_UP) || theApp.m_taskbar_data.display_item.Contains(TDI_DOWN));
        if (show_net_speed)
        {
            theApp.m_taskbar_data.display_item.Remove(TDI_UP);
            theApp.m_taskbar_data.display_item.Remove(TDI_DOWN);
        }
        else
        {
            theApp.m_taskbar_data.display_item.Add(TDI_UP);
            theApp.m_taskbar_data.display_item.Add(TDI_DOWN);
        }
        //CloseTaskBarWnd();
        //OpenTaskBarWnd();
        m_tBarDlg->WidthChanged();
    }
}


BOOL CTrafficMonitorDlg::OnQueryEndSession()
{
    if (!CDialog::OnQueryEndSession())
        return FALSE;

    // TODO:  Thêm mã chuyên biệt xử lý truy vấn kết thúc phiên tại đây
    theApp.SaveConfig();
    theApp.SaveGlobalConfig();
    SaveHistoryTrafficFull();  // Khi tắt máy thì dùng lưu đầy đủ, đảm bảo mọi dữ liệu đều được lưu
    BackupHistoryTrafficFile();

    if (theApp.m_debug_log)
    {
        CCommon::WriteLog(_T("Tiến trình TrafficMonitor đã bị kết thúc, thiết lập đã được lưu."), (theApp.m_config_dir + L".\\debug.log").c_str());
    }

    return TRUE;
}


void CTrafficMonitorDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
                       // TODO: Thêm mã xử lý thông điệp tại đây
                       // Không gọi CDialog::OnPaint() cho thông điệp vẽ
    m_skin.DrawInfo(&dc, theApp.m_cfg_data.m_show_more_info);
}


afx_msg LRESULT CTrafficMonitorDlg::OnDpichanged(WPARAM wParam, LPARAM lParam)
{
    static int dpi;
    static CTrafficMonitorDlg* pThis;
    dpi = LOWORD(wParam);
    pThis = this;

    //Vì khi kéo cửa sổ nổi sang màn hình có DPI khác sẽ kích hoạt nhiều thông điệp thay đổi DPI trong thời gian ngắn, nên ở đây sau khi nhận thông điệp sẽ trễ một khoảng rồi mới xử lý
    KillTimer(DPI_CHANGE_TIMER);
    SetTimer(DPI_CHANGE_TIMER, 500, [](HWND, UINT, UINT_PTR, DWORD) {
        //Lấy DPI theo vị trí cửa sổ chính
        CRect rect;
        pThis->GetWindowRect(rect);
        UINT dpi_x, dpi_y;
        if (theApp.DPIFromRect(rect, &dpi_x, &dpi_y))   //Lấy thành công thì dùng DPI lấy theo vị trí cửa sổ chính
            dpi = dpi_x;
        TRACE("Dpi changed: %d\n", dpi);

        theApp.SetDPI(dpi);
        //Khi phiên bản hệ thống thấp hơn Windows 8.1 thì dùng hành vi cũ
        if (pThis->IsTaskbarWndValid() && !theApp.m_win_version.IsWindows8Point1OrLater())
        {
            //Chỉ định lại DPI cho cửa sổ thanh tác vụ
            pThis->m_tBarDlg->SetDPI(dpi);
            //Thiết lập lại phông chữ cửa sổ thanh tác vụ theo DPI mới
            pThis->m_tBarDlg->SetTextFont();
        }

        pThis->LoadSkinLayout();   //Lấy dữ liệu bố cục theo skin đang chọn
        pThis->SetItemPosition();  //Khởi tạo vị trí cửa sổ
        pThis->LoadBackGroundImage();
        pThis->SetTextFont();      //Nạp lại phông chữ
        pThis->Invalidate(FALSE);  //Vẽ lại giao diện

        pThis->KillTimer(DPI_CHANGE_TIMER);
    });

    return 0;
}


afx_msg LRESULT CTrafficMonitorDlg::OnTaskbarWndClosed(WPARAM wParam, LPARAM lParam)
{
    theApp.m_cfg_data.m_show_task_bar_wnd = false;
    //Sau khi đóng cửa sổ thanh tác vụ, nếu không hiển thị biểu tượng vùng thông báo và không hiển thị cửa sổ chính hoặc đã bật cho chuột xuyên qua, thì hiển thị biểu tượng vùng thông báo
    if (!theApp.m_general_data.show_notify_icon && theApp.IsForceShowNotifyIcon())
    {
        AddNotifyIcon();
        theApp.m_general_data.show_notify_icon = true;
    }
    return 0;
}



afx_msg LRESULT CTrafficMonitorDlg::OnMonitorInfoUpdated(WPARAM wParam, LPARAM lParam)
{
    Invalidate(FALSE);      //Làm mới thông tin cửa sổ

    //Cập nhật chú giải khi trỏ chuột
    if (theApp.m_main_wnd_data.show_tool_tip && m_tool_tips.GetSafeHwnd() != NULL)
    {
        CString tip_info;
        tip_info = GetMouseTipsInfo();
        m_tool_tips.UpdateTipText(tip_info, this);
    }
    //Cập nhật chú giải chuột của cửa sổ thanh tác vụ
    if (IsTaskbarWndValid())
        m_tBarDlg->UpdateToolTips();
    return 0;
}


LRESULT CTrafficMonitorDlg::OnDisplaychange(WPARAM wParam, LPARAM lParam)
{
    GetScreenSize();
    CheckWindowPos(true);
    return 0;
}


void CTrafficMonitorDlg::OnExitSizeMove()
{
    // TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây
    CheckWindowPos();

    CDialog::OnExitSizeMove();
}


void CTrafficMonitorDlg::OnPluginManage()
{
    // TODO: Thêm mã xử lý lệnh tại đây
    CPluginManagerDlg dlg;
    dlg.DoModal();
}

LRESULT CTrafficMonitorDlg::OnReopenTaksbarWnd(WPARAM wParam, LPARAM lParam)
{
    CloseTaskBarWnd();
    OpenTaskBarWnd();
    return 0;
}


void CTrafficMonitorDlg::OnOpenTaskManager()
{
    ShellExecuteW(NULL, _T("open"), (theApp.m_system_dir + L"\\Taskmgr.exe").c_str(), NULL, NULL, SW_NORMAL);       //Mở Trình quản lý tác vụ (Task Manager)
}


void CTrafficMonitorDlg::RequestReinitConnection(ReinitReason reason)
{
    //Chỉ gửi một yêu cầu tại một thời điểm; luồng giao diện xử lý xong mới cho gửi tiếp
    bool expected = false;
    if (m_reinit_pending.compare_exchange_strong(expected, true))
    {
        if (!PostMessage(WM_REINIT_CONNECTION, static_cast<WPARAM>(reason), 0))
            m_reinit_pending = false;
    }
}

afx_msg LRESULT CTrafficMonitorDlg::OnReinitConnection(WPARAM wParam, LPARAM lParam)
{
    IniConnection();
    m_reinit_pending = false;
    const wchar_t* log_id = nullptr;
    switch (static_cast<ReinitReason>(wParam))
    {
    case REINIT_INSUFFICIENT_BUFFER: log_id = IDS_INSUFFICIENT_BUFFER; break;
    case REINIT_CONNECTION_NOT_MATCH: log_id = IDS_CONNECTION_NOT_MATCH; break;
    default: break;     //REINIT_INTERFACE_NUM_CHANGED đã được ghi nhật ký trong luồng giám sát
    }
    if (log_id != nullptr)
    {
        CString info = CCommon::LoadText(log_id);
        info.Replace(_T("<%cnt%>"), CCommon::IntToString(m_restart_cnt));
        CCommon::WriteLog(info, theApp.m_log_path.c_str());
    }
    return 0;
}

afx_msg LRESULT CTrafficMonitorDlg::OnSettingsApplied(WPARAM wParam, LPARAM lParam)
{
    COptionsDlg* pOptionsDlg = (COptionsDlg*)wParam;
    if (pOptionsDlg != nullptr)
    {
        ApplySettings(*pOptionsDlg);
    }
    return 0;
}


void CTrafficMonitorDlg::OnDisplaySettings()
{
    // TODO: Thêm mã xử lý lệnh tại đây
    CSetItemOrderDlg dlg;
    dlg.SetItemOrder(theApp.m_taskbar_data.item_order.GetItemOrderConst());
    dlg.SetDisplayItem(theApp.m_taskbar_data.display_item);
    dlg.SetPluginDisplayItem(theApp.m_taskbar_data.plugin_display_item);
    if (dlg.DoModal() == IDOK)
    {
        theApp.m_taskbar_data.item_order.SetOrder(dlg.GetItemOrder());
        theApp.m_taskbar_data.display_item = dlg.GetDisplayItem();
        theApp.m_taskbar_data.plugin_display_item = dlg.GetPluginDisplayItem();
        //CloseTaskBarWnd();
        //OpenTaskBarWnd();
        if (IsTaskbarWndValid())
            m_tBarDlg->WidthChanged();
    }
}


void CTrafficMonitorDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây
    //CheckClickedItem(point);
    //if (m_clicked_item.IsPlugin() && m_clicked_item.PluginItem() != nullptr)
    //{
    //    ITMPlugin* plugin = theApp.m_plugins.GetPluginByItem(m_clicked_item.PluginItem());
    //    if (plugin != nullptr && plugin->GetAPIVersion() >= 3)
    //    {
    //        if (m_clicked_item.PluginItem()->OnMouseEvent(IPluginItem::MT_LCLICKED, point.x, point.y, (void*)GetSafeHwnd(), 0) != 0)
    //            return;
    //    }
    //}

    CDialog::OnLButtonUp(nFlags, point);
}


void CTrafficMonitorDlg::OnRefreshConnectionList()
{
    IniConnection();

}


afx_msg LRESULT CTrafficMonitorDlg::OnTabletQuerysystemgesturestatus(WPARAM wParam, LPARAM lParam)
{
    return 0;
}


void CTrafficMonitorDlg::OnPluginOptions()
{
    if (m_clicked_item.IsPlugin())
    {
        //Tìm thấy plugin tương ứng
        ITMPlugin* plugin = theApp.m_plugins.GetPluginByItem(m_clicked_item.PluginItem());
        if (plugin != nullptr)
        {
            //Hiển thị tùy chọn của plugin
            auto rtn = plugin->ShowOptionsDialog(GetSafeHwnd());
            if (rtn == ITMPlugin::OR_OPTION_NOT_PROVIDED)
                MessageBox(CCommon::LoadText(IDS_PLUGIN_NO_OPTIONS_INFO), nullptr, MB_ICONINFORMATION | MB_OK);
        }
    }
}


void CTrafficMonitorDlg::OnPluginDetail()
{
    if (m_clicked_item.IsPlugin())
    {
        //Tìm thấy plugin tương ứng
        ITMPlugin* plugin = theApp.m_plugins.GetPluginByItem(m_clicked_item.PluginItem());
        if (plugin != nullptr)
        {
            int index = theApp.m_plugins.GetPluginIndex(plugin);
            CPluginInfoDlg dlg(index);
            dlg.DoModal();
        }
    }
}


void CTrafficMonitorDlg::OnPluginOptionsTaksbar()
{
    //Kiểm tra mục được nhấp trong cửa sổ thanh tác vụ có phải mục plugin không
    if (IsTaskbarWndValid() && m_tBarDlg->GetClickedItem().IsPlugin())
    {
        //Tìm thấy plugin tương ứng
        ITMPlugin* plugin = theApp.m_plugins.GetPluginByItem(m_tBarDlg->GetClickedItem().PluginItem());
        if (plugin != nullptr)
        {
            //Hiển thị tùy chọn của plugin
            auto rtn = plugin->ShowOptionsDialog(m_tBarDlg->GetSafeHwnd());
            if (rtn == ITMPlugin::OR_OPTION_CHANGED)    //Tùy chọn đã thay đổi, mở lại cửa sổ thanh tác vụ
            {
                //CloseTaskBarWnd();
                //OpenTaskBarWnd();
                m_tBarDlg->WidthChanged();
            }
            if (rtn == ITMPlugin::OR_OPTION_NOT_PROVIDED)
                MessageBox(CCommon::LoadText(IDS_PLUGIN_NO_OPTIONS_INFO), nullptr, MB_ICONINFORMATION | MB_OK);
        }
    }
}


void CTrafficMonitorDlg::OnPluginDetailTaksbar()
{
    if (IsTaskbarWndValid() && m_tBarDlg->GetClickedItem().IsPlugin())
    {
        //Tìm thấy plugin tương ứng
        ITMPlugin* plugin = theApp.m_plugins.GetPluginByItem(m_tBarDlg->GetClickedItem().PluginItem());
        if (plugin != nullptr)
        {
            int index = theApp.m_plugins.GetPluginIndex(plugin);
            CPluginInfoDlg dlg(index);
            dlg.DoModal();
        }
    }
}


UINT CTrafficMonitorDlg::OnPowerBroadcast(UINT nPowerEvent, LPARAM nEventData)
{
    // Hệ thống tiếp tục từ chế độ ngủ đông
    if (nPowerEvent == PBT_APMRESUMESUSPEND)
    {
        //Khởi tạo lại kết nối mạng sau một khoảng thời gian trễ
        KillTimer(INIT_CONNECT_TIMER);
        static CTrafficMonitorDlg* pThis = this;
        static int check_times = 0;
        SetTimer(INIT_CONNECT_TIMER, 10000, [](HWND, UINT, UINT_PTR, DWORD) {
            pThis->IniConnection();
            check_times++;

            //Ghi nhật ký
            CString info = CCommon::LoadTextFormat(IDS_RESTORE_FROM_SLEEP_LOG, {pThis->m_restart_cnt });
            CCommon::WriteLog(info, theApp.m_log_path.c_str());

            //Nếu không có kết nối, bộ định thời tiếp tục chạy và định kỳ khởi tạo lại kết nối
            if (pThis->m_connections.size() == 0)
            {
                //Quá 20 lần thì kết thúc bộ định thời
                if (check_times >= 20)
                    pThis->KillTimer(INIT_CONNECT_TIMER);
            }
            //Lấy được kết nối thành công, kết thúc bộ định thời
            else
            {
                pThis->KillTimer(INIT_CONNECT_TIMER);
                check_times = 0;
            }
        });
    }
    return CDialog::OnPowerBroadcast(nPowerEvent, nEventData);
}


void CTrafficMonitorDlg::OnColorizationColorChanged(DWORD dwColorizationColor, BOOL bOpacity)
{
    // Tính năng này yêu cầu Windows Vista trở lên.
    // Ký hiệu _WIN32_WINNT phải >= 0x0600.
    // TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây

    static DWORD last_color;
    if (last_color != dwColorizationColor)
    {
        last_color = dwColorizationColor;
        BYTE red = (dwColorizationColor >> 16) & 0xFF;
        BYTE green = (dwColorizationColor >> 8) & 0xFF;
        BYTE blue = dwColorizationColor & 0xFF;
        COLORREF theme_color = RGB(red, green, blue);
        theApp.SetThemeColor(theme_color);
    }
    CDialog::OnColorizationColorChanged(dwColorizationColor, bOpacity);
}
