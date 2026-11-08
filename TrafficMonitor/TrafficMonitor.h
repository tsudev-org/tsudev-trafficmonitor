
// TrafficMonitor.h : tệp header chính của ứng dụng PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
#error "Hãy include “stdafx.h” trước khi include tệp này để tạo tệp PCH"
#endif

#include "resource.h"       // Ký hiệu chính
#include "Common.h"
#include "WinVersionHelper.h"
#include "SimpleXML.h"
#include "TaskbarDefaultStyle.h"
#include <map>
#include "OpenHardwareMonitor/OpenHardwareMonitorApi.h"
#include "PluginManager.h"
#include "Nullable.hpp"
#include "TaskBarDlgDrawCommon.h"
#include "DllFunctions.h"
#include "StrTable.h"
#include "PluginUpdateHelper.h"
#include "PluginInterface.h"

// CTrafficMonitorApp:
// Về phần triển khai của lớp này, xem TrafficMonitor.cpp
//


class CTrafficMonitorApp : public CWinApp, public ITrafficMonitor
{
public:
    //Các đường dẫn
    static CTrafficMonitorApp* self;
    wstring m_module_dir;       //Thư mục chứa tệp exe của chương trình
    wstring m_appdata_dir;
    wstring m_module_path;      //Đường dẫn tệp exe của chương trình
    wstring m_module_path_reg;  //Đường dẫn tệp exe dùng để ghi vào mục tự khởi động trong Registry (nếu đường dẫn có khoảng trắng thì thêm dấu ngoặc kép)
    wstring m_config_path;
    wstring m_history_traffic_path;
    wstring m_log_path;
    wstring m_skin_path;
    wstring m_system_dir;
    wstring m_config_dir;

    //Các dữ liệu sau được định nghĩa là thành viên public của lớp App để có thể truy cập từ cả hộp thoại chính và cửa sổ thanh tác vụ
    unsigned __int64 m_in_speed{};      //Tốc độ tải xuống
    unsigned __int64 m_out_speed{};     //Tốc độ tải lên
    int m_cpu_usage{ -1 };      //Mức sử dụng CPU
    int m_memory_usage{ -1 };   //Mức sử dụng bộ nhớ
    int m_used_memory{};    //Bộ nhớ vật lý khả dụng (đơn vị KB)
    int m_total_memory{};   //Tổng bộ nhớ vật lý (đơn vị KB)
    float m_cpu_temperature{ -1 };  //Nhiệt độ CPU
    float m_cpu_freq{ -1 };  //Tần số CPU
    float m_gpu_temperature{ -1 };  //Nhiệt độ GPU
    float m_hdd_temperature{ -1 };  //Nhiệt độ ổ cứng
    float m_main_board_temperature{ -1 };    //Nhiệt độ bo mạch chủ
    int m_gpu_usage{ -1 };      //Mức sử dụng GPU
    int m_hdd_usage{ -1 };      //Mức sử dụng ổ cứng

    unsigned __int64 m_today_up_traffic{};  //Lưu lượng tải lên đã dùng hôm nay
    unsigned __int64 m_today_down_traffic{};    //Lưu lượng tải xuống đã dùng hôm nay

    bool m_cannot_save_config_warning{ true };  //Cho biết có hiện hộp thông báo khi không lưu được thiết lập hay không
    bool m_cannot_save_global_config_warning{ true };   //Cho biết có hiện hộp thông báo khi không lưu được thiết lập hay không

    bool m_module_dir_writable{ true };         //Cho biết thư mục chứa chương trình có ghi được hay không

    //Dữ liệu tùy chọn
    MainWndSettingData m_main_wnd_data;
    TaskBarSettingData m_taskbar_data;
    GeneralSettingData m_general_data;
    //Dữ liệu thiết lập khác
    MainConfigData m_cfg_data;
    int m_notify_interval;      //Khoảng thời gian giữa các lần hiện thông báo
    bool m_debug_log{};
    bool m_taksbar_transparent_color_enable{};
    bool m_last_light_mode{};
    bool m_show_mouse_panetrate_tip{};  //Có hiển thị thông báo khi bật “Cho chuột xuyên qua” hay không.
    bool m_show_dot_net_notinstalled_tip{};

    //bool m_is_windows10_fall_creator;
    CWinVersionHelper m_win_version;        //Phiên bản Windows hiện tại

    HICON m_notify_icons[MAX_NOTIFY_ICON];

    CTaskbarDefaultStyle m_taskbar_default_style;
    CPluginManager m_plugins;
    CDllFunctions m_dll_functions;
    CStrTable m_str_table;
    CPluginUpdateHelper m_plugin_update;

    CMenu m_main_menu;          //Menu chuột phải của cửa sổ chính
    CMenu m_main_menu_plugin;   //Menu chuột phải khi nhấp vào vùng plugin của cửa sổ chính
    CMenu m_main_menu_plugin_sub_menu;
    CMenu m_taskbar_menu;       //Menu chuột phải của cửa sổ thanh tác vụ
    CMenu m_taskbar_menu_plugin;    //Menu chuột phải khi nhấp vào vùng plugin của cửa sổ thanh tác vụ
    CMenu m_taskbar_menu_plugin_sub_menu;

#ifndef WITHOUT_TEMPERATURE
    //Con trỏ tới giao diện OpenHardwareMonitor
    std::shared_ptr<OpenHardwareMonitorApi::IOpenHardwareMonitor> m_pMonitor{};
#endif // !WITHOUT_TEMPERATURE

    CCriticalSection m_minitor_lib_critical;        //Đối tượng critical section dùng để đồng bộ luồng khi truy cập OpenHardwareMonitor
    //CCriticalSection m_lftable_critical;            //Đối tượng critical section dùng để đồng bộ luồng khi truy cập LfTable2
    CLazyConstructable<class CTaskBarDlgDrawCommonSupport> m_d2d_taskbar_draw_common_support{}; // Phụ thuộc toàn cục được tự động khởi tạo khi dùng kết xuất D2D

public:
    CTrafficMonitorApp();

    void LoadLanguageConfig();
    void LoadConfig();
    void SaveConfig();
    void LoadPluginDisabledSettings();

    void LoadGlobalConfig();
    void SaveGlobalConfig();

    int DPI(int pixel);
    void DPI(CRect& rect);
    void DPIFromWindow(CWnd* pWnd);
    int GetDpi() const { return m_dpi; }
    void SetDPI(int dpi) { m_dpi = dpi; }

    void CheckUpdate(bool message);     //Kiểm tra cập nhật; nếu message là true thì hiện thông báo trong khi kiểm tra
    void CheckUpdateInThread(bool message); //Kiểm tra cập nhật trong luồng nền
    //Hàm luồng kiểm tra cập nhật khi khởi động
    static UINT CheckUpdateThreadFunc(LPVOID lpParam);
    static UINT InitOpenHardwareMonitorLibThreadFunc(LPVOID lpParam);

    bool SetAutoRun(bool auto_run, bool task_scheduler);
    bool GetAutoRun(wstring* auto_run_path, bool task_scheduler);        //Kiểm tra có tự khởi động cùng Windows không; nếu có thì ghi đường dẫn tự khởi động vào auto_run_path

    bool SetAutoRunByRegistry(bool auto_run);       //Tự khởi động cùng Windows qua Registry
    bool SetAutoRunByTaskScheduler(bool auto_run);  //Tự khởi động cùng Windows qua Task Scheduler

    //Lấy văn bản thông tin hệ thống
    CString GetSystemInfoString();

    void InitMenuResourse();

    //Lấy một tài nguyên biểu tượng; nếu tài nguyên chưa được nạp thì tự động nạp.
    //Vì hàm này dùng hàm CTrafficMonitorApp::DPI nên phải đảm bảo hàm này được gọi sau CTrafficMonitorApp::DPIFromWindow
    HICON GetMenuIcon(UINT id);

    void AutoSelectNotifyIcon();

    bool IsCheckingForUpdate() const { return m_checking_update; }      //Có đang kiểm tra cập nhật hay không

    void InitOpenHardwareLibInThread();     //Mở một luồng nền để khởi tạo OpenHardwareMonitor
    void UpdateOpenHardwareMonitorEnableState();    //Cập nhật trạng thái bật/tắt của giám sát phần cứng

    //void UpdateTaskbarWndMenu();      //Cập nhật menu chuột phải của cửa sổ thanh tác vụ
    bool IsForceShowNotifyIcon();       //Có cần buộc hiển thị biểu tượng vùng thông báo hay không

    std::wstring GetPlauginTooltipInfo() const;
    bool IsTaksbarItemDisplayed(CommonDisplayItem item) const;

    void SendSettingsToPlugin();    //Gửi thiết lập tùy chọn hiện tại tới tất cả plugin

    //Cập nhật menu con của plugin
    //plugin_cmd_start_index: vị trí bắt đầu của lệnh plugin trong menu
    static void UpdatePluginMenu(CMenu* pMenu, ITMPlugin* plugin, int plugin_cmd_start_index);

    void CheckWindows11Taskbar();
    bool IsWindows11Taskbar() const { return m_is_windows11_taskbar; }

    bool DPIFromRect(const RECT& rect, UINT* out_dpi_x, UINT* out_dpi_y);

    virtual unsigned int GetThemeColor() const override;
    void SetThemeColor(COLORREF color);

private:
    //int m_no_multistart_warning_time{};       //Dùng để thiết lập trong khoảng thời gian bao lâu sau khi khởi động máy thì không hiện cảnh báo “Đã có một chương trình đang chạy”
    bool m_no_multistart_warning{};         //Nếu là false thì sẽ không bao giờ hiện cảnh báo “Đã có một chương trình đang chạy”
    bool m_exit_when_start_by_restart_manager{ true };      //Nếu chương trình được Restart Manager của Windows khởi động lại thì thoát chương trình
    int m_dpi{ 96 };

    bool m_checking_update{ false };        //Có đang kiểm tra cập nhật hay không

    std::map<UINT, HICON> m_menu_icons;      //Tài nguyên biểu tượng menu. key là ID tài nguyên biểu tượng, value là handle của biểu tượng

    ULONG_PTR m_gdiplusToken{};

    bool m_is_windows11_taskbar{ false };  //Có phải thanh tác vụ của Windows 11 hay không
    COLORREF m_theme_color{};

// Ghi đè
public:
    virtual BOOL InitInstance();

    // Phần triển khai

    DECLARE_MESSAGE_MAP()
    afx_msg void OnHelp();
    afx_msg void OnFrequentyAskedQuestions();
    afx_msg void OnUpdateLog();
    virtual int ExitInstance();

    // Kế thừa qua ITrafficMonitor
private:
    virtual int GetAPIVersion() override;
    virtual const wchar_t* GetVersion() override;
    double GetMonitorValue(MonitorItem item) override;
    virtual const wchar_t* GetMonitorValueString(MonitorItem item, int is_main_window = false) override;
    virtual const wchar_t* GetStringRes(const wchar_t* key, const wchar_t* section) override;
public:
    void ShowNotifyMessage(const wchar_t* strMsg) override;
    unsigned short GetLanguageId() const override;
    const wchar_t* GetPluginConfigDir() const override;
    int GetDPI(DPIType type) const override;
    virtual void* GetMainWindowHwnd() override;
    virtual void* GetTaskbarWindowHwnd() override;
};

extern CTrafficMonitorApp theApp;
