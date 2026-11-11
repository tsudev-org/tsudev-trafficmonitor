#pragma once
#include <functional>

// Hộp thoại CBaseDialog
//Dùng để ghi nhớ kích thước hộp thoại
//đồng thời đặt kích thước ban đầu của cửa sổ bằng kích thước tối thiểu

class CBaseDialog : public CDialog
{
    DECLARE_DYNAMIC(CBaseDialog)

public:
    CBaseDialog(UINT nIDTemplate, CWnd* pParent = NULL);   // Hàm khởi tạo chuẩn
    virtual ~CBaseDialog();

    // Sao chép từ CDialogEx, chức năng giống hệt (bổ sung xử lý cho điều khiển thanh trượt và siêu liên kết)
    void SetBackgroundColor(COLORREF color, BOOL bRepaint = TRUE);

    // Dữ liệu hộp thoại
    //#ifdef AFX_DESIGN_TIME
    //	enum { IDD = IDD_BASEDIALOG };
    //#endif

    void SetMinSize(int cx, int cy);		//Thiết lập kích thước tối thiểu của cửa sổ; nếu chưa thiết lập thì dùng kích thước ban đầu làm kích thước tối thiểu
    static HWND GetUniqueHandel(LPCTSTR dlg_name);          //Lấy handle duy nhất của cửa sổ chỉ định
    static const std::map<CString, HWND>& AllUniqueHandels();   //Lấy handle của tất cả cửa sổ
    static bool IsAllDialogClosed();

private:
    void LoadConfig();
    void SaveConfig() const;
    void IterateControls(CWnd* pParent, std::function<void(CWnd*)> func);
    // Áp dụng lại tham số trình quản lý bố cục, khiến kích thước/vị trí gốc của điều khiển lấy theo trạng thái hiện tại
    void ReLoadLayoutResource();

private:
    UINT m_nDialogID;                                   // Biến thành viên dùng để lưu ID tài nguyên
    CSize m_min_size{};     //Kích thước tối thiểu của cửa sổ (lưu theo kích thước ở 96 dpi)
    CSize m_window_size{ -1, -1 };
    CBrush m_brBkgr;
    CDC* m_pDC = nullptr;                               // Có hiệu lực trong InitializeControls, dùng để đo độ dài văn bản
    static std::map<CString, HWND> m_unique_hwnd;       //Handle cửa sổ duy nhất cho mỗi lớp cơ sở
    bool m_remember_dlg_size{ true };                   //Có ghi nhớ kích thước cửa sổ hay không (khi cờ này là true và GetDialogName trả về chuỗi không rỗng thì sẽ ghi nhớ kích thước cửa sổ)
    CFont m_dlg_font;
    int m_dpi;              //DPI riêng của cửa sổ

protected:
    int DPI(int pixel) const;

    // Chỉ dùng được trong InitializeControls; đo độ dài văn bản của điều khiển
    CRect GetTextExtent(const CString& text);

    struct CtrlTextInfo
    {
        // Enum ở đây được dùng như kiểu int có kiểm tra kiểu
        // Giá trị Col cụ thể không có ý nghĩa, chỉ biểu thị thứ tự tương đối giữa các cột và những điều khiển nào cùng cột; thực tế không giới hạn số lượng, có thể tăng nếu cần
        enum Col { L4 = -4, L3 = -3, L2 = -2, L1 = -1, C0 = 0, R1 = 1, R2 = 2, R3 = 3, R4 = 4, UN_USE = 100 };
        // Chiều rộng còn lại của điều khiển sau khi trừ phần văn bản.
        // Văn bản tĩnh Static nên dùng W0, nút bấm có thể có biểu tượng nên dùng W32
        enum Width { W_50 = -50, W0 = 0, W16 = 16, W24 = 24, W32 = 32, W40 = 40, W60 = 60, W64 = 64, W96 = 96, W128 = 128, W256 = 256 };

        Col col_index{ UN_USE };    // Chỉ vị trí của điều khiển, tăng dần từ trái sang phải; nhỏ hơn 0 là căn trái, lớn hơn 0 là căn phải, bằng 0 là dùng khoảng trống còn lại
        UINT id{ 0 };               // ID điều khiển
        Width ext_width{ W0 };      // Chiều rộng điều khiển tối thiểu phải bằng “chiều rộng văn bản + ext_width” (bên trong sẽ chuyển đổi DPI theApp.DPI(ext_width))
    };
    // Chỉ dùng được trong InitializeControls; sắp xếp lại điều khiển theo độ dài văn bản, không điều chỉnh theo chiều dọc, không thay đổi khoảng cách giữa các điều khiển
    // Chỉ làm tăng chiều rộng điều khiển, nên khuyến nghị đặt chiều rộng nhỏ hơn trong tài nguyên rồi dùng phương thức này để điều chỉnh cho phù hợp
    // Ưu tiên đảm bảo chiều rộng còn lại ở giữa (hoặc chiều rộng điều khiển có col_index bằng 0) tối thiểu là center_min_width
    // Khi thiếu chỗ, văn bản của các điều khiển khác sẽ không hiển thị đầy đủ (lúc đó nên thiết kế lại cửa sổ) (phương thức này chỉ phù hợp khi văn bản không thể quá dài)
    void RepositionTextBasedControls(const vector<CtrlTextInfo>& items, CtrlTextInfo::Width center_min_width = CtrlTextInfo::W16);
    
    // Được CBaseDialog::OnInitDialog gọi trước khi khôi phục kích thước cửa sổ từ cấu hình
    // Lớp dẫn xuất thực hiện khởi tạo một phần điều khiển, như thiết lập văn bản, sắp xếp lại điều khiển bằng RepositionTextBasedControls
    // Trả về true sẽ áp dụng lại tham số của trình quản lý bố cục, khiến kích thước/vị trí gốc của bố cục động lấy theo trạng thái hiện tại
    // Các khởi tạo liên quan đến kích thước cửa sổ thực tế (như độ rộng cột bảng) nên thực hiện trong OnInitDialog của lớp dẫn xuất
    virtual bool InitializeControls() { return false; };

    virtual CString GetDialogName() const { return CString(); }
    void EnableDlgCtrl(UINT id, bool enable);
    void SetButtonIcon(UINT id, HICON hIcon);
    void SetRememberDlgSize(bool enable) { m_remember_dlg_size = enable; }
    CRect GetControlRect(CWnd* pCtrl);
    CRect GetControlRect(UINT id);

    //Duyệt tất cả điều khiển con
    void IterateControls(std::function<void(CWnd*)> func);

    virtual void DoDataExchange(CDataExchange* pDX);    // Hỗ trợ DDX/DDV

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    virtual INT_PTR DoModal();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
