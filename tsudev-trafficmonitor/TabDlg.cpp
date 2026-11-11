#include "stdafx.h"
#include "TabDlg.h"
#include "TsudevTrafficMonitor.h"

IMPLEMENT_DYNAMIC(CTabDlg, CBaseDialog)

CTabDlg::CTabDlg(UINT nIDTemplate, CWnd * pParent) : CBaseDialog(nIDTemplate, pParent)
{
    m_pParent = pParent;
}


CTabDlg::~CTabDlg()
{
}


BOOL CTabDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Thêm mã chuyên biệt và/hoặc gọi lớp cơ sở tại đây
    //Vì đây là hộp thoại con trong thẻ tab, khi người dùng nhấn Enter hoặc ESC không nên phản hồi IDOK hoặc IDCANCEL của hộp thoại hiện tại,
    //mà nên phản hồi lệnh IDOK hoặc IDCANCEL của cửa sổ cha chứa thẻ tab
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
    {
        if (m_pParent != nullptr)
            m_pParent->SendMessage(WM_COMMAND, IDCANCEL);
        return TRUE;
    }
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
        if (m_pParent != nullptr)
            m_pParent->SendMessage(WM_COMMAND, IDOK);
        return TRUE;
    }

    return CBaseDialog::PreTranslateMessage(pMsg);
}


BOOL CTabDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  Thêm khởi tạo bổ sung tại đây
    //Đặt nền cửa sổ thành màu trắng
    if (theApp.m_win_version.IsWindows11OrLater())
        SetBackgroundColor(RGB(249, 249, 249));
    else
        SetBackgroundColor(RGB(255, 255, 255));

    return TRUE;  // return TRUE unless you set the focus to a control
                  // Ngoại lệ: trang thuộc tính OCX phải trả về FALSE
}

CWnd* CTabDlg::GetParentWindow()
{
    CWnd* pParent = GetParent();
    if (pParent != nullptr)
    {
        return pParent->GetParent();
    }
    return nullptr;
}

void CTabDlg::SetScrollbarInfo(int nPage, int nMax)
{
    // Lấy trạng thái thanh cuộn
    SCROLLINFO scrollinfo;
    GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
    if (m_scroll_enable)
        m_last_pos = scrollinfo.nPos - scrollinfo.nMin;     // Nếu đã bật cuộn thì ghi lại vị trí cuộn hiện tại
    else
        m_scroll_enable = true;                             // Ngược lại thì bật cuộn
    int reset_pos = m_last_pos;
    // Kiểm tra biên của trạng thái cuộn mới sau OnSize của cửa sổ cha
    if (nMax < 0) nMax = 0;
    if (m_last_pos > nMax - nPage) m_last_pos = nMax - nPage;
    if (m_last_pos < 0) m_last_pos = 0;
    // Thiết lập trạng thái thanh cuộn
    scrollinfo.nMin = 0;                // Thiết lập pos điểm bắt đầu của nội dung cuộn trong cửa sổ
    scrollinfo.nMax = nMax;             // Thiết lập pos điểm kết thúc của nội dung cuộn trong cửa sổ
    scrollinfo.nPage = nPage;           // Thiết lập chiều cao vùng nội dung hiển thị thực tế
    scrollinfo.nPos = m_last_pos;       // Thiết lập pos đỉnh của vùng nội dung đang hiển thị thực tế
    SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
    // Đặt lại cuộn
    if (reset_pos != 0)
        ScrollWindow(0, reset_pos);
    // Thay framework phản hồi OnSize (sau khi SetScrollInfo đã quyết định và thiết lập thanh cuộn có hiển thị ở trạng thái mới hay không)
    CBaseDialog::ResizeDynamicLayout(); // Phương thức này bỏ qua thanh cuộn khi cập nhật vị trí điều khiển khiến điều khiển bị lệch, gọi khi vị trí cuộn bằng 0 để tránh vấn đề này
    // Thực hiện cuộn
    if (m_last_pos != 0)
        ScrollWindow(0, -m_last_pos);
}

void CTabDlg::ScrollWindowSimple(int step, bool absolute)
{
    SCROLLINFO scrollinfo;
    GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
    if (step == INT_MAX)
        step = scrollinfo.nPage;
    if (step == INT_MIN)
        step = -static_cast<int>(scrollinfo.nPage);
    int offset_diff = scrollinfo.nMax - static_cast<int>(scrollinfo.nPage);
    if (offset_diff < 0)    //nMax luôn phải lớn hơn hoặc bằng nPage; khi nMax bằng nPage thì không có thanh cuộn
        offset_diff = 0;
    if (!absolute)
        scrollinfo.nPos -= step;
    else
    {
        if (step < 0)               // Với vị trí tuyệt đối, step bằng -1 nghĩa là cuộn xuống dưới cùng
            step = offset_diff;
        scrollinfo.nPos = step;     // Lúc này step là vị trí tuyệt đối đích
        step = m_last_pos - step;   // Chuyển vị trí tuyệt đối của tham số thành độ lệch để xử lý tiếp
    }
    if (scrollinfo.nPos < scrollinfo.nMin)                                          // Giới hạn nPos không nhỏ hơn giá trị tối thiểu nMin
    {
        step -= scrollinfo.nMin - scrollinfo.nPos;
        scrollinfo.nPos = scrollinfo.nMin;
    }
    if (scrollinfo.nPos > offset_diff)     // Giới hạn nPos không lớn hơn giá trị tối đa nMax - nPage
    {
        step += scrollinfo.nPos - offset_diff;
        scrollinfo.nPos = offset_diff;
    }
    SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
    m_last_pos = scrollinfo.nPos;
    ScrollWindow(0, step);
}

void CTabDlg::EnableDlgCtrl(UINT id, bool enable)
{
    CWnd* pCtrl = GetDlgItem(id);
    if (pCtrl != nullptr)
        pCtrl->EnableWindow(enable);
}

void CTabDlg::ShowDlgCtrl(UINT id, bool show)
{
    CWnd* pCtrl = GetDlgItem(id);
    if (pCtrl != nullptr)
        pCtrl->ShowWindow(show);
}

BEGIN_MESSAGE_MAP(CTabDlg, CBaseDialog)
    ON_WM_VSCROLL()
    ON_WM_MOUSEWHEEL()
    ON_WM_SIZE()
END_MESSAGE_MAP()


void CTabDlg::ResizeDynamicLayout()
{
    // m_pDynamicLayout->Adjust() sẽ bỏ qua thanh cuộn của cửa sổ khi cập nhật vị trí điều khiển
    // Trước đây đặt lại sau OnSize của cửa sổ cha không ổn định (không hoạt động đúng khi vào/thoát phóng to và khi bám cạnh), cũng chưa tìm được cách phát hiện hai kiểu OnSize đặc biệt này
    // Framework gọi ResizeDynamicLayout quá sớm, không kịp đặt lại cuộn trước, nên ghi đè phương thức ảo này để can thiệp

    // Nếu đã bật cuộn thì ở đây không gọi lớp cơ sở để vô hiệu xử lý của framework, thay vào đó tự gọi sau OnSize của cửa sổ cha
    if (!m_scroll_enable)
        CBaseDialog::ResizeDynamicLayout();
}

void CTabDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây
    //Tài liệu tham khảo: https://www.cnblogs.com/ranjiewen/p/6013922.html

    if (m_scroll_enable)
    {
        int step = theApp.DPI(16);
        switch (nSBCode)
        {
        case SB_LINEUP:                         //Scroll one line up
            ScrollWindowSimple(step);
            break;
        case SB_LINEDOWN:                       //Scroll one line down
            ScrollWindowSimple(-step);
            break;
        case SB_PAGEUP:                         //Scroll one page up.
            ScrollWindowSimple(INT_MAX);
            break;
        case SB_PAGEDOWN:                       //Scroll one page down
            ScrollWindowSimple(INT_MIN);
            break;
        case SB_THUMBPOSITION:                  //Scroll to the absolute position. The current position is provided in nPos
            break;
        case SB_THUMBTRACK:                     //Drag scroll box to specified position. The current position is provided in nPos
            ScrollWindowSimple(nPos, true);
            break;
        case SB_TOP:                            // Trên cùng
            ScrollWindowSimple(0, true);
            break;
        case SB_BOTTOM:                         // Dưới cùng
            ScrollWindowSimple(-1, true);
            break;
        case SB_ENDSCROLL:                      //End scroll
            break;
        default:
            break;
        }
    }
    CBaseDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


BOOL CTabDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây
    if (m_scroll_enable)
    {
        int step = theApp.DPI(64) * zDelta / 120;
        ScrollWindowSimple(step);
        SCROLLINFO scrollinfo;
        GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
        m_last_pos = scrollinfo.nPos;
    }

    return CBaseDialog::OnMouseWheel(nFlags, zDelta, pt);
}


void CTabDlg::OnOK()
{
    // TODO: Thêm mã chuyên biệt và/hoặc gọi lớp cơ sở tại đây

    CBaseDialog::OnOK();
}


bool CTabDlg::IsScrollBarVisible()
{
    SCROLLINFO scrollinfo;
    GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
    bool visible = scrollinfo.nMax > 0 && scrollinfo.nPage < static_cast<UINT>(scrollinfo.nMax);
    return visible;
}

void CTabDlg::OnSize(UINT nType, int cx, int cy)
{
    CBaseDialog::OnSize(nType, cx, cy);

    // TODO: Thêm mã xử lý thông điệp tại đây
    SetControlMouseWheelEnable(!IsScrollBarVisible());      //Nếu đang hiển thị thanh cuộn thì cấm điều khiển phản hồi con lăn chuột, lúc này con lăn chuột dùng để cuộn cửa sổ
}
