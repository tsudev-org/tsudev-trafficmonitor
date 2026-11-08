
// TrafficMonitorDlg.h : tệp header
//

#pragma once
#pragma comment (lib, "iphlpapi.lib")

#include "NetworkInfoDlg.h"
#include "afxwin.h"
#include "StaticEx.h"
#include "Common.h"
#include "TaskBarDlg.h"
#include "SkinDlg.h"
#include "HistoryTrafficDlg.h"
#include "OptionsDlg.h"
#include "PictureStatic.h"
#include "IconSelectDlg.h"
#include "DrawCommon.h"
#include "IniHelper.h"
#include "LinkStatic.h"
#include "AdapterCommon.h"
#include "AboutDlg.h"
#include "PdhHardwareQuery/CPUUsage.h"
#include "PdhHardwareQuery/CpuFreq.h"
#include <afxmt.h>
#include <atomic>
#include "PdhHardwareQuery/GpuUsage.h"
#include "PdhHardwareQuery/DiskUsage.h"
#include "HistoryTrafficFile.h"

// Hộp thoại CTrafficMonitorDlg
class CTrafficMonitorDlg : public CDialog
{
    // Hàm khởi tạo
public:
    CTrafficMonitorDlg(CWnd* pParent = NULL);   // Hàm khởi tạo chuẩn
    ~CTrafficMonitorDlg();
    CTaskBarDlg* GetTaskbarWindow() const;

    static CTrafficMonitorDlg* Instance();

    // Dữ liệu hộp thoại
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_TRAFFICMONITOR_DIALOG };
#endif

    const CSkinFile& GetCurSkin() const { return m_skin; }

    CPdhDiskUsage& GetPdhDiskUsageHelper() { return m_disk_usage_helper; }
    bool IsGetDiskUsageByPdh() const { return m_get_disk_usage_by_pdh; }

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // Hỗ trợ DDX/DDV


// Phần triển khai
protected:
    HICON m_hIcon;
    NOTIFYICONDATA m_ntIcon;    //Biểu tượng vùng thông báo
    CTaskBarDlg* m_tBarDlg{};     //Con trỏ tới cửa sổ thanh tác vụ

    vector<NetWorkConection> m_connections; //Lưu tất cả kết nối mạng đã lấy được để hiển thị trong mục menu “Chọn card mạng”
    MIB_IFTABLE* m_pIfTable{};
    DWORD m_dwSize{};	//Kích thước của m_pIfTable
    //Bảo vệ m_pIfTable và m_connections: luồng giám sát đọc chúng, còn IniConnection (chỉ chạy trên luồng giao diện) cấp phát lại.
    CCriticalSection m_connections_lock;
    std::atomic<bool> m_reinit_pending{ false };    //Đã gửi WM_REINIT_CONNECTION nhưng luồng giao diện chưa xử lý
    bool m_last_counters_valid{ false };            //Bộ đếm byte của lần lấy trước có hợp lệ (64-bit) hay không
    int m_connection_selected{ 0 }; //Số thứ tự kết nối cần hiển thị lưu lượng
    unsigned __int64 m_in_bytes{};        //Số byte đã nhận hiện tại
    unsigned __int64 m_out_bytes{};   //Số byte đã gửi hiện tại
    unsigned __int64 m_last_in_bytes{}; //Số byte đã nhận lần trước
    unsigned __int64 m_last_out_bytes{};    //Số byte đã gửi lần trước

    CCPUUsage m_cpu_usage_helper;
    CPdhCpuFreq m_cpu_freq_helper;
    CPdhGPUUsage m_gpu_usage_helper;
    CPdhDiskUsage m_disk_usage_helper;

    bool m_get_disk_usage_by_pdh{};

    bool m_first_start{ true };     //Ban đầu là true, đặt thành false sau khi bộ định thời chạy lần đầu

    // https://www.jianshu.com/p/9d4b68cdbd99
    struct Monitors
    {
        std::vector<MONITORINFO> monitorinfos;

        static BOOL CALLBACK MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData)
        {
            MONITORINFO iMonitor;
            iMonitor.cbSize = sizeof(MONITORINFO);
            GetMonitorInfo(hMon, &iMonitor);

            Monitors* pThis = reinterpret_cast<Monitors*>(pData);
            pThis->monitorinfos.push_back(iMonitor);
            return TRUE;
        }

        Monitors()
        {
            EnumDisplayMonitors(0, 0, MonitorEnum, (LPARAM)this);
        }
    };

    //CRect m_screen_rect;        //Phạm vi màn hình (không gồm thanh tác vụ)
    vector<CRect> m_screen_rects;       //Phạm vi của tất cả màn hình (không gồm thanh tác vụ)
    vector<CRect> m_last_screen_rects;       //Phạm vi của tất cả màn hình lần trước (không gồm thanh tác vụ)
    CSize m_screen_size;        //Kích thước màn hình (gồm thanh tác vụ)
    CSkinFile m_skin;
    CommonDisplayItem m_clicked_item;           //Mục được nhấp chuột

    int m_restart_cnt{ -1 };    //Số lần khởi tạo lại
    unsigned int m_timer_cnt{};     //Số lần bộ định thời kích hoạt (số giây kể từ khi chương trình khởi động)
    unsigned int m_taskbar_timer_cnt{0}; //Số lần bộ định thời kích hoạt dùng cho TaskBarDlg (số giây kể từ khi chương trình khởi động)
    unsigned int m_monitor_time_cnt{};
    int m_zero_speed_cnt{}; //Nếu không phát hiện được tốc độ mạng, biến này sẽ tự tăng
    int m_insert_to_taskbar_cnt{};  //Dùng để đếm số lần thử nhúng vào thanh tác vụ
    int m_cannot_insert_to_task_bar_warning{ true };   //Cho biết có hiện hộp thông báo khi không nhúng được vào thanh tác vụ hay không

    static unsigned int m_WM_TASKBARCREATED;    //Thông điệp khởi động lại thanh tác vụ

    int m_skin_selected{};      //Số thứ tự skin đã chọn

    SYSTEMTIME m_start_time;    //Thời điểm chương trình khởi động
    CHistoryTrafficFile m_history_traffic{ theApp.m_history_traffic_path }; //Lưu lưu lượng lịch sử

    CToolTipCtrl m_tool_tips;

    bool m_connection_change_flag{ false };     //Nếu đã chạy hàm IniConnection() thì flag này sẽ được đặt thành true
    bool m_is_foreground_fullscreen{ false };   //Cho biết cửa sổ tiền cảnh có đang hiển thị toàn màn hình hay không
    bool m_menu_popuped{ false };               //Cho biết hiện có menu nào đang bật lên hay không

    HDC m_desktop_dc;

    string m_connection_name_preferd{ theApp.m_cfg_data.m_connection_name };          //Lưu tên kết nối mạng do người dùng chọn thủ công

    void DoMonitorAcquisition();    //Lấy thông tin giám sát một lần
    static UINT MonitorThreadCallback(LPVOID dwUser);   //Hàm luồng lấy thông tin giám sát
    bool m_monitor_data_required{ false };          //Cờ cần lấy dữ liệu giám sát trong luồng; đặt thành true khi cần lấy dữ liệu giám sát, đặt thành false khi đã lấy được một lần
    bool m_is_thread_exit{ false }; //Cờ thoát luồng
    CEvent m_threadExitEvent;       //Dùng để báo cho luồng chính biết luồng làm việc đã thoát
public:
    void ExitMonitorThread();       //Dừng luồng giám sát

protected:
    CString GetMouseTipsInfo();     //Lấy thông tin chú giải chuột
    void SetTransparency();         //Thiết lập độ trong suốt cửa sổ theo giá trị m_transparency
    void SetTransparency(int transparency);
public:
    void SetAlwaysOnTop();          //Ghim cửa sổ trên cùng theo giá trị m_always_on_top
protected:
    void SetMousePenetrate();       //Thiết lập cho chuột xuyên qua theo giá trị m_mouse_penetrate
    POINT CalculateWindowMoveOffset(CRect rect, bool screen_changed);  //Tính vị trí cần di chuyển tới khi cửa sổ nằm ngoài vùng màn hình
    void CheckWindowPos(bool screen_changed = false);          //Kiểm tra vị trí cửa sổ; nếu cửa sổ nằm ngoài màn hình thì di chuyển để toàn bộ cửa sổ nằm trong màn hình và trả về vị trí mới
    void GetScreenSize();           //Lấy kích thước màn hình

    void AutoSelect();
    //void UpdateConnections();
    //Tự động chọn kết nối
    void IniConnection();   //Khởi tạo kết nối (chỉ gọi trên luồng giao diện)
    enum ReinitReason { REINIT_INSUFFICIENT_BUFFER, REINIT_INTERFACE_NUM_CHANGED, REINIT_CONNECTION_NOT_MATCH };
    void RequestReinitConnection(ReinitReason reason);  //Luồng giám sát gọi để yêu cầu luồng giao diện khởi tạo lại kết nối

    MIB_IFROW GetConnectIfTable(int connection_index);    //Lấy đối tượng MIB_IFROW của kết nối mạng đang chọn. connection_index là chỉ số trong m_connections
    NetWorkConection GetConnection(int connection_index); //Lấy đối tượng NetWorkConection của kết nối mạng đang chọn. connection_index là chỉ số trong m_connections

    void IniConnectionMenu(CMenu* pMenu);   //Khởi tạo menu “Chọn kết nối mạng”
    void IniTaskBarConnectionMenu();        //Khởi tạo menu “Chọn kết nối mạng” của cửa sổ thanh tác vụ
    void SetConnectionMenuState(CMenu* pMenu);      //Thiết lập mục được chọn trong menu “Chọn kết nối mạng”

    void CloseTaskBarWnd(); //Đóng cửa sổ thanh tác vụ
    void OpenTaskBarWnd();  //Mở cửa sổ thanh tác vụ

    void AddNotifyIcon();       //Thêm biểu tượng vùng thông báo
    void DeleteNotifyIcon();
public:
    void ShowNotifyTip(const wchar_t* title, const wchar_t* message);       //Hiển thị thông báo ở vùng thông báo
protected:
    void UpdateNotifyIconTip();     //Cập nhật chú giải chuột của biểu tượng vùng thông báo

    void SaveHistoryTraffic();        // Lưu tăng dần, chỉ cập nhật dòng đầu và bản ghi hôm nay
    void SaveHistoryTrafficFull();    // Lưu đầy đủ, dùng khi chương trình thoát để đảm bảo mọi dữ liệu đều được lưu
    void LoadHistoryTraffic();
    void BackupHistoryTrafficFile();

    void _OnOptions(int tab, CWnd* pParent);   //Xử lý mở hộp thoại “Tùy chọn”, tab: thẻ được chuyển tới khi mở

    void ApplySettings(COptionsDlg& optionsDlg);

    void SetItemPosition();     //Thiết lập vị trí của 4 mục hiển thị
    bool LoadSkinLayout();      //Lấy dữ liệu bố cục từ skin hiện tại

    void LoadBackGroundImage();
    void SetTextFont();

public:
    bool IsTaskbarWndValid() const;
protected:

    void TaskbarShowHideItem(DisplayItem type);

    //Xác định một điểm nằm trong vùng của mục hiển thị nào và lưu vào m_clicked_item
    void CheckClickedItem(CPoint point);

    //Áp dụng một skin
    void ApplySkin(int skin_index);

public:
    //void ApplySettings();
    bool IsTemperatureNeeded() const;       //Kiểm tra có cần hiển thị thông tin nhiệt độ không

protected:
    // Các hàm ánh xạ thông điệp được sinh ra
    virtual BOOL OnInitDialog();
    //  afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    //  afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    //  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnNetworkInfo();
    afx_msg void OnAlwaysOnTop();
    afx_msg void OnTransparency100();
    afx_msg void OnTransparency80();
    afx_msg void OnTransparency60();
    afx_msg void OnTransparency40();
    afx_msg void OnClose();
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    afx_msg void OnInitMenu(CMenu* pMenu);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnLockWindowPos();
    afx_msg void OnMove(int x, int y);
protected:
    afx_msg LRESULT OnNotifyIcon(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnShowNotifyIcon();
    afx_msg void OnDestroy();
    afx_msg void OnShowCpuMemory();
    afx_msg void OnMousePenetrate();
    //afx_msg void OnTextColor();
    afx_msg void OnShowTaskBarWnd();
    afx_msg void OnAppAbout();
    afx_msg void OnShowCpuMemory2();
    afx_msg void OnShowMainWnd();
    afx_msg void OnChangeSkin();
    afx_msg LRESULT OnTaskBarCreated(WPARAM wParam, LPARAM lParam);
    //afx_msg void OnSetFont();
    afx_msg void OnTrafficHistory();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnOptions();
    afx_msg void OnOptions2();
protected:
    afx_msg LRESULT OnExitmenuloop(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnChangeNotifyIcon();
    afx_msg void OnAlowOutOfBorder();
    afx_msg void OnCheckUpdate();
protected:
    afx_msg LRESULT OnTaskbarMenuPopedUp(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnShowNetSpeed();
    afx_msg BOOL OnQueryEndSession();
    afx_msg void OnPaint();
protected:
    afx_msg LRESULT OnDpichanged(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnTaskbarWndClosed(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnMonitorInfoUpdated(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDisplaychange(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnReopenTaksbarWnd(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnExitSizeMove();
    afx_msg void OnPluginManage();
    afx_msg void OnOpenTaskManager();
protected:
    afx_msg LRESULT OnSettingsApplied(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnReinitConnection(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnDisplaySettings();
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnRefreshConnectionList();
protected:
    afx_msg LRESULT OnTabletQuerysystemgesturestatus(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnPluginOptions();
    afx_msg void OnPluginDetail();
    afx_msg void OnPluginOptionsTaksbar();
    afx_msg void OnPluginDetailTaksbar();
    afx_msg UINT OnPowerBroadcast(UINT nPowerEvent, LPARAM nEventData);
    afx_msg void OnColorizationColorChanged(DWORD dwColorizationColor, BOOL bOpacity);
};
