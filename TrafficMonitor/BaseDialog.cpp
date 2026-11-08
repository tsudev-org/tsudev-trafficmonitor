// BaseDialog.cpp : tệp triển khai
//

#include "stdafx.h"
#include "BaseDialog.h"
#include "afxdialogex.h"
#include "IniHelper.h"
#include "TrafficMonitor.h"
#include "TrafficMonitorDlg.h"

// Hộp thoại CBaseDialog
std::map<CString, HWND> CBaseDialog::m_unique_hwnd;

IMPLEMENT_DYNAMIC(CBaseDialog, CDialog)

CBaseDialog::CBaseDialog(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
    : CDialog(nIDTemplate, pParent)
{
    m_nDialogID = nIDTemplate;
    m_dpi = theApp.GetDpi();
}

CBaseDialog::~CBaseDialog()
{
}

void CBaseDialog::SetBackgroundColor(COLORREF color, BOOL bRepaint)
{
    if (m_brBkgr.GetSafeHandle() != NULL)
    {
        m_brBkgr.DeleteObject();
    }

    if (color != (COLORREF)-1)
    {
        m_brBkgr.CreateSolidBrush(color);
    }

    if (bRepaint && GetSafeHwnd() != NULL)
    {
        Invalidate();
        UpdateWindow();
    }
}

void CBaseDialog::SetMinSize(int cx, int cy)
{
    m_min_size.cx = cx;
    m_min_size.cy = cy;
}

HWND CBaseDialog::GetUniqueHandel(LPCTSTR dlg_name)
{
    return m_unique_hwnd[dlg_name];
}

const std::map<CString, HWND>& CBaseDialog::AllUniqueHandels()
{
    return m_unique_hwnd;
}

bool CBaseDialog::IsAllDialogClosed()
{
    for (const auto& hwnd : m_unique_hwnd)
    {
        if (hwnd.second != nullptr)
            return false;
    }
    return true;
}

void CBaseDialog::LoadConfig()
{
    if (!GetDialogName().IsEmpty() && m_remember_dlg_size)
    {
        CIniHelper ini{ theApp.m_config_path };
        //Tải thiết lập kích thước cửa sổ
        m_window_size.cx = ini.GetInt(_T("window_size"), GetDialogName() + _T("_width"), -1);
        m_window_size.cy = ini.GetInt(_T("window_size"), GetDialogName() + _T("_height"), -1);
    }
}

void CBaseDialog::SaveConfig() const
{
    if (!GetDialogName().IsEmpty() && m_remember_dlg_size)
    {
        CIniHelper ini{ theApp.m_config_path };
        //Lưu thiết lập kích thước cửa sổ
        ini.WriteInt(_T("window_size"), GetDialogName() + _T("_width"), m_window_size.cx);
        ini.WriteInt(_T("window_size"), GetDialogName() + _T("_height"), m_window_size.cy);
        ini.Save();
    }
}

void CBaseDialog::IterateControls(CWnd* pParent, std::function<void(CWnd*)> func)
{
    if (pParent == nullptr)
        return;

    // Lấy điều khiển con đầu tiên
    CWnd* pChild = pParent->GetWindow(GW_CHILD);

    // Duyệt tất cả điều khiển con
    while (pChild != nullptr)
    {
        func(pChild);

        // Duyệt đệ quy điều khiển con của điều khiển con (xử lý điều khiển lồng nhau)
        IterateControls(pChild, func);

        // Lấy điều khiển anh em kế tiếp
        pChild = pChild->GetWindow(GW_HWNDNEXT);
    }
}

void CBaseDialog::ReLoadLayoutResource()
{
    ASSERT(m_nDialogID);
    CMFCDynamicLayout* pDynamicLayout = GetDynamicLayout();
    if (pDynamicLayout)
    {
        HRSRC layoutRes = ::FindResourceW(NULL, MAKEINTRESOURCEW(m_nDialogID), RT_DIALOG_LAYOUT);
        if (layoutRes)
        {
            HGLOBAL hResData = ::LoadResource(NULL, layoutRes);
            if (hResData)
            {
                LPVOID layoutResData = ::LockResource(hResData);
                DWORD layoutResSize = ::SizeofResource(NULL, layoutRes);
                // std::wstring data(static_cast<wchar_t*>(layoutResData), layoutResSize / sizeof(wchar_t));
                pDynamicLayout->LoadResource(this, layoutResData, layoutResSize);
            }
        }
    }
}

int CBaseDialog::DPI(int pixel) const
{
    return m_dpi * pixel / 96;
}

CRect CBaseDialog::GetTextExtent(const CString& text)
{
    ASSERT(m_pDC != nullptr);   // m_pDC do OnInitDialog chịu trách nhiệm cấp phát và giải phóng
    if (m_pDC == nullptr)
        return CRect();
    if (text.IsEmpty())
        return CRect();
    CRect text_size;
    m_pDC->DrawTextW(text, &text_size, DT_CALCRECT);    // Dùng CDC::DrawTextW để đo chiều rộng văn bản (CDC::GetTextExtent là chiều rộng lý thuyết, không chính xác)
    return text_size;
}

void CBaseDialog::RepositionTextBasedControls(const vector<CtrlTextInfo>& items, CtrlTextInfo::Width center_min_width)
{
    ASSERT(m_pDC != nullptr);   // Phương thức này chỉ dùng được trong InitializeControls
    if (m_pDC == nullptr)
        return;
    int center_width = theApp.DPI(center_min_width);
    std::map<int, std::pair<int, int>> col_info;
    struct itemInfo
    {
        int col_index;
        CWnd* p;
        CRect rect;
    };
    vector<itemInfo> items_info;
    int center_left{ 0 }, center_right{ INT_MAX };

    for (const auto& item : items)
    {
        ASSERT(item.col_index != CtrlTextInfo::UN_USE);
        ASSERT(item.id != 0);
        // Lấy dx cần cho mọi cột, cùng mép phải center_left của các phần tử căn trái và mép trái center_right của các phần tử căn phải
        CWnd* pItem = GetDlgItem(item.id);
        if (pItem == nullptr)
            continue;
        CRect rect{};
        pItem->GetWindowRect(&rect);
        ScreenToClient(&rect);
        CString text;
        pItem->GetWindowTextW(text);
        int dx = GetTextExtent(text).Width() + theApp.DPI(item.ext_width) - rect.Width();
        if (dx < 0) dx = 0;                                     // Văn bản chỉ làm tăng chiều rộng điều khiển
        if (col_info[item.col_index].first < dx)                // Lấy phần tử có độ rộng tăng nhiều nhất trong cột này
            col_info[item.col_index].first = dx;
        if (item.col_index < 0 && center_left < rect.right)
            center_left = rect.right;
        if (item.col_index > 0 && center_right > rect.left)
            center_right = rect.left;
        if (item.col_index == 0)    // Có thể có nhiều điều khiển với col_index bằng 0
        {
            if (center_left < rect.left)
                center_left = rect.left;
            if (center_right > rect.right)
                center_right = rect.right;
        }
        items_info.emplace_back(itemInfo{ item.col_index, pItem, std::move(rect) });
    }

    if (center_right == INT_MAX)    // Nếu tất cả điều khiển đều căn trái thì lấy mép phải cửa sổ làm mép phải của khoảng trống còn lại
    {
        CRect dlg_rect{};
        GetClientRect(&dlg_rect);
        center_right = dlg_rect.Width();
    }
    // Assert này kích hoạt nghĩa là bố cục gốc trong tệp tài nguyên không chừa đủ chiều rộng cho điều khiển giữa/khoảng trống
    ASSERT(center_right - center_left >= center_width);
    int dx_sum_left{}, dx_sum_right{};
    // Vì cùng một col_index có thể có nhiều điều khiển và không yêu cầu thứ tự, nên vị trí cuối cùng của điều khiển phải tính được mà không phụ thuộc trạng thái
    for (auto& a : col_info)
    {
        if (a.first < 0)
        {
            a.second.second = dx_sum_left;    // Lưu tổng dx của các điều khiển trước cột này, tức là khoảng dịch sang phải của các điều khiển trong cột này
            dx_sum_left += a.second.first;
        }
        else if (a.first > 0)
        {
            a.second.second = dx_sum_right;
            dx_sum_right += a.second.first;
        }
    }
    float scale{ 1.0f };
    if (center_right - center_left - dx_sum_left - dx_sum_right < center_width)
    {
        // ASSERT(false);
        // Văn bản vừa tải làm điều khiển giữa/khoảng trống của dòng này bị ép quá nhỏ
        // Cần thiết kế lại bố cục điều khiển để phù hợp với độ dài bản dịch hiện tại; tạm thời chia đều phần thiếu hụt cho các dx
        scale = static_cast<float>(center_right - center_left - center_width) / (dx_sum_left + dx_sum_right);
        dx_sum_left = static_cast<int>(dx_sum_left * scale + 0.5f);
        dx_sum_right = static_cast<int>(dx_sum_right * scale + 0.5f);
    }
    for (const auto& item : items_info)
    {
        const auto& rect = item.rect;
        int dx = static_cast<int>(col_info[item.col_index].first * scale + 0.5f);
        int sum_dx = static_cast<int>(col_info[item.col_index].second * scale + 0.5f);
        if (item.col_index < 0)
            item.p->SetWindowPos(nullptr, rect.left + sum_dx, rect.top, rect.Width() + dx, rect.Height(), SWP_NOZORDER);
        else if (item.col_index > 0)
            item.p->SetWindowPos(nullptr, rect.left + sum_dx - dx_sum_right, rect.top, rect.Width() + dx, rect.Height(), SWP_NOZORDER);
        else if (item.col_index == 0)
            item.p->SetWindowPos(nullptr, rect.left + dx_sum_left, rect.top, rect.Width() - dx_sum_left - dx_sum_right, rect.Height(), SWP_NOZORDER);
    }
}

void CBaseDialog::EnableDlgCtrl(UINT id, bool enable)
{
    CWnd* pWnd = GetDlgItem(id);
    if (pWnd != nullptr)
        pWnd->EnableWindow(enable);
}

void CBaseDialog::SetButtonIcon(UINT id, HICON hIcon)
{
    CWnd* dlgItem = GetDlgItem(id);
    CButton* btn = static_cast<CButton*>(dlgItem);
    if (btn != nullptr)
        btn->SetIcon(hIcon);
}

CRect CBaseDialog::GetControlRect(CWnd* pCtrl)
{
    if (pCtrl != nullptr)
    {
        CRect rect;
        pCtrl->GetWindowRect(rect);
        ScreenToClient(rect);
        return rect;
    }
    return CRect();
}

CRect CBaseDialog::GetControlRect(UINT id)
{
    CWnd* pCtrl = GetDlgItem(id);
    return GetControlRect(pCtrl);
}

void CBaseDialog::IterateControls(std::function<void(CWnd*)> func)
{
    func(this);
    IterateControls(this, func);
}

void CBaseDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBaseDialog, CDialog)
    ON_WM_DESTROY()
    ON_WM_GETMINMAXINFO()
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// Trình xử lý thông điệp của CBaseDialog


BOOL CBaseDialog::OnInitDialog()
{
    m_unique_hwnd[GetDialogName()] = m_hWnd;
    CDialog::OnInitDialog();

    // TODO:  Thêm khởi tạo bổ sung tại đây

    //Khởi tạo phông chữ
    CRect rect;
    GetWindowRect(rect);
    FontInfo font_info;
    font_info.name = theApp.m_str_table.GetLanguageInfo().default_font_name.c_str();
    font_info.size = 9;
    UINT dpi_x{}, dpi_y{};
    if (theApp.DPIFromRect(rect, &dpi_x, &dpi_y))
        m_dpi = dpi_x;

    font_info.Create(m_dlg_font, m_dpi);

    //Lấy kích thước ban đầu của cửa sổ
    if (m_min_size.cx <= 0 || m_min_size.cy <= 0)
    {
        CRect rect;
        GetWindowRect(rect);
        m_min_size.cx = rect.Width() * 96 / theApp.GetDpi();
        m_min_size.cy = rect.Height() * 96 / theApp.GetDpi();
    }

    //Tải thiết lập
    LoadConfig();

    SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);       //Bỏ ghim trên cùng

    //Xử lý dịch văn bản trong hộp thoại 
    IterateControls([&](CWnd* pWnd) {
        //Thiết lập phông chữ cho điều khiển
        if (m_dlg_font.GetSafeHandle() != NULL)
            pWnd->SetFont(&m_dlg_font);
        //Lấy văn bản của điều khiển
        CString str;
        pWnd->GetWindowText(str);
        UINT id = pWnd->GetDlgCtrlID();
        if (str.Left(4) == _T("TXT_"))
        {
            //Thiết lập văn bản cho điều khiển
            const std::wstring& str_translated = theApp.m_str_table.LoadText(str.GetString());
            if (!str_translated.empty())
                pWnd->SetWindowTextW(str_translated.c_str());
        }
        //Xử lý các nút chuẩn
        else
        {
            if (id == IDOK)
                pWnd->SetWindowTextW(theApp.m_str_table.LoadText(TXT_OK).c_str());
            else if (id == IDCANCEL)
                pWnd->SetWindowTextW(theApp.m_str_table.LoadText(TXT_CANCEL).c_str());
            else if (id == IDCLOSE)
                pWnd->SetWindowTextW(theApp.m_str_table.LoadText(TXT_CLOSE).c_str());
        }
    });

    // Trước khi khôi phục kích thước cửa sổ (trạng thái cửa sổ hiện tại khớp với tài nguyên), lớp dẫn xuất khởi tạo văn bản điều khiển và điều chỉnh bố cục điều khiển
    // Các khởi tạo liên quan đến kích thước cửa sổ thực tế (như độ rộng cột bảng) nên thực hiện trong OnInitDialog của lớp dẫn xuất
    m_pDC = GetDC();
    m_pDC->SelectObject(&m_dlg_font);
    bool rtn = InitializeControls();
    ReleaseDC(m_pDC);
    m_pDC = nullptr;
    // Nếu đã thay đổi bố cục điều khiển thì nên trả về true để áp dụng điều chỉnh vào trình quản lý bố cục (nạp lại thiết lập bố cục động)
    if (rtn)
        ReLoadLayoutResource();


    //Khởi tạo kích thước cửa sổ
    if (m_window_size.cx > 0 && m_window_size.cy > 0)
    {
        SetWindowPos(nullptr, 0, 0, m_window_size.cx, m_window_size.cy, SWP_NOZORDER | SWP_NOMOVE);
    }

    return TRUE;  // return TRUE unless you set the focus to a control
                  // Ngoại lệ: trang thuộc tính OCX phải trả về FALSE
}


void CBaseDialog::OnDestroy()
{
    CDialog::OnDestroy();

    // TODO: Thêm mã xử lý thông điệp tại đây
    m_unique_hwnd[GetDialogName()] = NULL;
    SaveConfig();

    //Đặt lại cửa sổ chính luôn trên cùng khi tất cả hộp thoại đã đóng
    CTrafficMonitorDlg* pDlg = dynamic_cast<CTrafficMonitorDlg*>(theApp.m_pMainWnd);
    if (pDlg != nullptr && IsAllDialogClosed())
        pDlg->SetAlwaysOnTop();
}


void CBaseDialog::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây
    //Giới hạn kích thước tối thiểu của cửa sổ
    lpMMI->ptMinTrackSize.x = theApp.DPI(m_min_size.cx);		//Thiết lập chiều rộng tối thiểu
    lpMMI->ptMinTrackSize.y = theApp.DPI(m_min_size.cy);		//Thiết lập chiều cao tối thiểu

    CDialog::OnGetMinMaxInfo(lpMMI);
}


void CBaseDialog::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // TODO: Thêm mã xử lý thông điệp tại đây
    if (nType != SIZE_MAXIMIZED && nType != SIZE_MINIMIZED)
    {
        //m_window_width = cx;
        //m_window_hight = cy;
        CRect rect;
        GetWindowRect(&rect);
        m_window_size.cx = rect.Width();
        m_window_size.cy = rect.Height();
    }

}


INT_PTR CBaseDialog::DoModal()
{
    HWND unique_hwnd{ m_unique_hwnd[GetDialogName()] };
    if (unique_hwnd != NULL && !GetDialogName().IsEmpty())      ///Nếu hộp thoại đã tồn tại thì hiển thị hộp thoại đó
    {
        ::ShowWindow(unique_hwnd, SW_RESTORE);
        ::SetForegroundWindow(unique_hwnd);
        return 0;
    }
    return CDialog::DoModal();
}


BOOL CBaseDialog::OnEraseBkgnd(CDC* pDC)
{
    // Thay đổi nền cửa sổ (CDialogEx)
    if (m_brBkgr.GetSafeHandle() != NULL)
    {
        ASSERT_VALID(pDC);
        CRect rectClient;
        GetClientRect(rectClient);
        pDC->FillRect(rectClient, &m_brBkgr);
        return TRUE;
    }
    return CDialog::OnEraseBkgnd(pDC);
}


HBRUSH CBaseDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    // Thay đổi nền cửa sổ (CDialogEx)
    if (m_brBkgr.GetSafeHandle() != NULL)
    {
#define AFX_MAX_CLASS_NAME 255
#define AFX_STATIC_CLASS _T("Static")
#define AFX_BUTTON_CLASS _T("Button")
#define AFX_SLIDER_CLASS _T("msctls_trackbar32")    // Điều khiển thanh trượt CSliderCtrl và các lớp dẫn xuất
#define AFX_SYSLINK_CLASS _T("SysLink")             // Điều khiển siêu liên kết CSysLink và các lớp dẫn xuất

        if (nCtlColor == CTLCOLOR_STATIC)
        {
            TCHAR lpszClassName[AFX_MAX_CLASS_NAME + 1];

            ::GetClassName(pWnd->GetSafeHwnd(), lpszClassName, AFX_MAX_CLASS_NAME);
            CString strClass = lpszClassName;

            if (strClass == AFX_BUTTON_CLASS || strClass == AFX_STATIC_CLASS || strClass == AFX_SLIDER_CLASS || strClass == AFX_SYSLINK_CLASS)
            {
                pDC->SetBkMode(TRANSPARENT);

                if (m_brBkgr.GetSafeHandle() != NULL && IsAppThemed())
                {
                    return (HBRUSH)m_brBkgr.GetSafeHandle();
                }
                else
                {
                    return (HBRUSH)::GetStockObject(HOLLOW_BRUSH);
                }
            }
        }
    }

    return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}
