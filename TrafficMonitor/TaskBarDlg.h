#pragma once
#include "Common.h"
#include "afxwin.h"
#include "DrawCommon.h"
#include "TaskBarDlgDrawCommon.h"
#include "TrafficMonitor.h"
#include "IniHelper.h"
#include "CommonData.h"
#include "TaskbarItemOrderHelper.h"
#include "SupportedRenderEnums.h"
#include <list>

// Hộp thoại CTaskBarDlg
#define TASKBAR_WND_HEIGHT DPI(32)				//Chiều cao của cửa sổ thanh tác vụ
#define WM_TASKBAR_MENU_POPED_UP (WM_USER + 1004)		//Định nghĩa thông điệp được gửi khi menu chuột phải của cửa sổ thanh tác vụ bật lên
//#define TASKBAR_GRAPH_MAX_LEN 600						//Độ dài tối đa lưu dữ liệu lịch sử
#define TASKBAR_GRAPH_STEP 5							//Mấy giây vẽ một đường

class CTaskBarDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CTaskBarDlg)

public:
    CTaskBarDlg(CWnd* pParent = NULL);   // Hàm khởi tạo chuẩn
    virtual ~CTaskBarDlg();

    CToolTipCtrl m_tool_tips;

    void ShowInfo(CDC* pDC); 	//Vẽ thông tin lên điều khiển
    void TryDrawStatusBar(IDrawCommon& drawer, const CRect& rect_bar, int usage_percent); //Vẽ thanh trạng thái CPU/bộ nhớ

    void TryDrawGraph(IDrawCommon& drawer, const CRect& value_rect, CommonDisplayItem item_type);		// Vẽ biểu đồ động CPU/bộ nhớ

    bool AdjustWindowPos(bool force_adjust = false);	//Thiết lập vị trí cửa sổ trên thanh tác vụ (nếu force_adjust là true thì sẽ buộc điều chỉnh vị trí cửa sổ thanh tác vụ một lần)
    void ApplyWindowTransparentColor();

    //bool IsTaskbarChanged();

    void WidthChanged();    //Gọi hàm này để thông báo chiều rộng cửa sổ thanh tác vụ đã thay đổi, buộc điều chỉnh vị trí cửa sổ thanh tác vụ một lần

    //Lấy vùng hình chữ nhật dùng để kiểm tra DPI
    const CRect& GetRectForDpiCheck() const;

    UINT GetDPI() const;
    void SetDPI(UINT dpi);
    UINT DPI(UINT pixel) const;
    int DPI(int pixel) const;
    LONG DPI(LONG pixel) const;
    void DPI(CRect& rect) const;

    //Định nghĩa lớp dưới đây có thể xem như định nghĩa hàm, tránh việc các thực thể khác nhau của hàm template sinh ra các biến tĩnh khác nhau.
    // Gần như khai báo hàm
    // template <class HanlderFunc>
    // CheckWindowMonitorDPIAndHandle(CTaskBarDlg& ref_taskbar_window, HanlderFunc handler)
    static class ClassCheckWindowMonitorDPIAndHandle
    {
    public:
        template <class HandlerFunc>
        void operator()(CTaskBarDlg& ref_taskbar_window, HandlerFunc handler)
        {
            bool rtn = theApp.DPIFromRect(ref_taskbar_window.GetRectForDpiCheck(), &dpi_x, &dpi_y);
            //Chỉ lấy dpi_x làm DPI của chương trình
            if (rtn && (dpi_x != buffered_dpi_x || dpi_y != buffered_dpi_y))
            {
                //Cập nhật dữ liệu bộ nhớ đệm
                buffered_dpi_x = dpi_x;
                buffered_dpi_y = dpi_y;
                //Gọi phương thức xử lý do người dùng tự định nghĩa
                handler(dpi_x, dpi_y);
            }
        }

    private:
        static UINT buffered_dpi_x, buffered_dpi_y;
        static UINT dpi_x, dpi_y;
    } CheckWindowMonitorDPIAndHandle;

    // Dữ liệu hộp thoại
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_TASK_BAR_DIALOG };
#endif

protected:
    virtual void InitTaskbarWnd() = 0;
    virtual void AdjustTaskbarWndPos(bool force_adjust) = 0;
    virtual void ResetTaskbarPos() = 0;
    virtual void CheckTaskbarOnTopOrBottom() = 0;		//Kiểm tra thanh tác vụ có nằm ở trên cùng hoặc dưới cùng màn hình không và lưu kết quả vào m_taskbar_on_top_or_bottom
    virtual HWND GetParentHwnd() = 0;

    virtual void DoDataExchange(CDataExchange* pDX);    // Hỗ trợ DDX/DDV

    HWND m_hTaskbar;	//Handle của cửa sổ thanh tác vụ
    CRect m_rcTaskbar;  //Vùng hình chữ nhật của thanh tác vụ
    CRect m_rect;		//Vùng hình chữ nhật của cửa sổ hiện tại
    int m_window_width{};   //Lưu chiều rộng cửa sổ đã tính
    int m_window_height{};  //Lưu chiều rộng và chiều cao đã tính
    CSupportedRenderEnums m_supported_render_enums{};
    DefaultCLazyConstructableWithInitializer<
        CTaskBarDlgDrawCommonWindowSupport,
        CTaskBarDlgDrawCommonSupport&>
        m_taskbar_draw_common_window_support{[]() -> std::tuple<CTaskBarDlgDrawCommonSupport&>
                                             { return {theApp.m_d2d_taskbar_draw_common_support.Get()}; }}; //Cung cấp hỗ trợ vẽ D2D1
    DefaultCLazyConstructableWithInitializer<
        CD2D1DeviceContextWindowSupport,
        CTaskBarDlgDrawCommonSupport&>
        m_d2d1_device_context_support{[]() -> std::tuple<CTaskBarDlgDrawCommonSupport&>
                                      { return {theApp.m_d2d_taskbar_draw_common_support.Get()}; }};

    //Chiều rộng các phần của thanh tác vụ
    struct ItemWidth
    {
        int label_width{};      //Chiều rộng phần nhãn
        int value_width{};      //Chiều rộng phần giá trị

        int TotalWidth() const  //Tổng chiều rộng
        {
            return label_width + value_width;
        }

        int MaxWidth() const	//Chiều rộng lớn nhất
        {
            return max(label_width, value_width);
        }
    };

    struct ItemWidthInfo : public CommonDisplayItem
    {
        ItemWidth item_width;
        bool is_double_line{};  //Mục có chiếm hai dòng hay không

        ItemWidthInfo()
        {}

        ItemWidthInfo(const CommonDisplayItem& item)
            : CommonDisplayItem(item)
        {}
    };

    std::vector<ItemWidthInfo> m_item_widths;   //Chiều rộng thực tế hiển thị của từng phần trong cửa sổ thanh tác vụ
    std::map<CommonDisplayItem, CRect> m_item_rects;    //Vùng hình chữ nhật của từng phần trong cửa sổ thanh tác vụ
    CommonDisplayItem m_clicked_item;           //Mục trên thanh tác vụ được nhấp chuột

    std::map<CommonDisplayItem, std::list<int>> m_map_history_data;  //Danh sách liên kết lưu dữ liệu lịch sử của các mục; lưu theo thứ tự thời gian, càng gần đầu thì dữ liệu càng mới
    std::map<CommonDisplayItem, int> m_history_data_count;            //Đếm số lần thêm vào danh sách liên kết dữ liệu lịch sử

    bool m_connot_insert_to_task_bar{ false };	//Là true nếu cửa sổ không nhúng được vào thanh tác vụ
    bool m_taskbar_on_top_or_bottom{ true };		//Là true nếu thanh tác vụ nằm ở trên cùng hoặc dưới cùng màn hình
    int m_error_code{};
    bool m_menu_popuped{ false };               //Cho biết hiện có menu nào đang bật lên hay không
    bool m_is_secondary_display{ false };       //Có hiển thị trên màn hình phụ hay không
    bool m_is_width_changed{ false };

    UINT m_taskbar_dpi{};//DPI dùng riêng cho TaskBarDlg

    CFont m_font;			//Phông chữ

    CDC* m_pDC{};		//DC của cửa sổ, dùng để tính chiều rộng cửa sổ

    //Tìm handle của thanh tác vụ
    //is_scendary_display: thanh tác vụ tìm được có thuộc màn hình phụ hay không
    HWND FindTaskbarHandle(bool& is_scendary_display);
    CString GetMouseTipsInfo();		//Lấy chú giải chuột

    void AddHisToList(CommonDisplayItem item_type, int current_usage_percent);		//Thêm giá trị mức sử dụng hiện tại vào danh sách liên kết

    int CalculateNetspeedPercent(unsigned __int64 net_speed);     //Tính phần trăm của tốc độ mạng so với giá trị lớn nhất của biểu đồ tốc độ mạng

    //Xác định một điểm nằm trong vùng của mục hiển thị nào và lưu vào m_clicked_item. Nếu trả về false thì điểm đó không nằm trong vùng của mục nào, ngược lại trả về true
    bool CheckClickedItem(CPoint point);

    //Vẽ một mục hiển thị trong cửa sổ thanh tác vụ
    //  drawer: đối tượng của lớp vẽ
    //  type: loại mục
    //  rect: vùng hình chữ nhật để vẽ
    //  label_width: chiều rộng vùng nhãn
    //  vertical: nếu là true thì nhãn và giá trị hiển thị trên dưới
    void DrawDisplayItem(IDrawCommon& drawer, DisplayItem type, CRect rect, int label_width, bool vertical = false);

    //Vẽ một mục plugin trong cửa sổ thanh tác vụ
   //  drawer: đối tượng của lớp vẽ
   //  item: con trỏ tới mục hiển thị của plugin
   //  rect: vùng hình chữ nhật để vẽ
   //  label_width: chiều rộng vùng nhãn
   //  vertical: nếu là true thì nhãn và giá trị hiển thị trên dưới
    void DrawPluginItem(IDrawCommon& drawer, IPluginItem* item, CRect rect, int label_width, bool vertical = false);

    void MoveWindow(CRect rect);

public:
    static void DisableRenderFeatureIfNecessary(CSupportedRenderEnums& ref_supported_render_enums);
    void SetTextFont();
    void ApplySettings();
    void CalculateWindowSize();		//Tính kích thước từng phần của cửa sổ và chiều rộng các phần. Kích thước cửa sổ lưu vào m_window_width và m_window_height, chiều rộng các phần lưu vào m_item_widths, vùng hình chữ nhật các phần lưu vào m_item_rects

    void SetToolTipsTopMost();			//Đặt chú giải chuột luôn trên cùng
    void UpdateToolTips();

    bool GetCannotInsertToTaskBar() const { return m_connot_insert_to_task_bar; }
    int GetErrorCode() const { return m_error_code; }
    bool IsTasksbarOnTopOrBottom() const { return m_taskbar_on_top_or_bottom; }

    static bool IsItemShow(DisplayItem item);
    static bool IsShowCpuMemory();
    static bool IsShowNetSpeed();

    CommonDisplayItem GetClickedItem() const { return m_clicked_item; }

    //Có cho phép tùy chọn “Cửa sổ thanh tác vụ gần biểu tượng thay vì hai bên thanh tác vụ” hay không
    //taskbar_wnd_on_left: cửa sổ thanh tác vụ có nằm bên trái thanh tác vụ hay không
    static bool IsTaskbarCloseToIconEnable(bool taskbar_wnd_on_left);

    DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    virtual void OnCancel();
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    //afx_msg void OnSetBackgroundColor();
    //afx_msg void OnSetTextColor();
    afx_msg void OnInitMenu(CMenu* pMenu);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    //afx_msg void OnSetFont();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg void OnClose();
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
protected:
    afx_msg LRESULT OnExitmenuloop(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnTabletQuerysystemgesturestatus(WPARAM wParam, LPARAM lParam);
public:
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
