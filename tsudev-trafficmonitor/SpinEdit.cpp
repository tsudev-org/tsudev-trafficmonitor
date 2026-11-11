// SpinEdit.cpp: tệp triển khai
//

#include "stdafx.h"
#include "SpinEdit.h"

#define SPIN_ID 3100
// CSpinEdit

IMPLEMENT_DYNAMIC(CSpinEdit, CEdit)

CSpinEdit::CSpinEdit()
{

}

CSpinEdit::~CSpinEdit()
{
}

void CSpinEdit::SetRange(short lower, short upper, short step)
{
    m_step = step;
    if (m_spin.GetSafeHwnd() != NULL)
        m_spin.SetRange(lower, upper);
}

void CSpinEdit::SetValue(int value)
{
    ASSERT(m_step != 0);    // Nên thiết lập Range trước
    if (m_spin.GetSafeHwnd() != NULL)
        m_spin.SetPos(value);
}

int CSpinEdit::GetValue()
{
    if (m_spin.GetSafeHwnd() != NULL)
        return m_spin.GetPos32();
    else
        return 0;
}


void CSpinEdit::SetMouseWheelEnable(bool enable)
{
    m_mouse_wheel_enable = enable;
}


BEGIN_MESSAGE_MAP(CSpinEdit, CEdit)
    ON_MESSAGE(WM_TABLET_QUERYSYSTEMGESTURESTATUS, &CSpinEdit::OnTabletQuerysystemgesturestatus)
    ON_NOTIFY(UDN_DELTAPOS, SPIN_ID, &CSpinEdit::OnDeltaposSpin)
    ON_WM_KILLFOCUS()
    ON_WM_ENABLE()
    ON_WM_SIZE()
END_MESSAGE_MAP()



// Trình xử lý thông điệp của CSpinEdit

void CSpinEdit::PreSubclassWindow()
{
    // TODO: Thêm mã chuyên biệt và/hoặc gọi lớp cơ sở tại đây

    // Yêu cầu văn bản căn trái, phía phải sẽ bị điều khiển spin che
    // WS_CLIPCHILDREN loại trừ vùng cửa sổ con khi vẽ, tránh nhấp nháy khi vẽ các điều khiển chồng lên nhau
    ModifyStyle(ES_CENTER | ES_RIGHT, ES_LEFT | WS_CLIPCHILDREN);
    m_spin.Create(WS_CHILD | WS_VISIBLE | UDS_ARROWKEYS | UDS_HOTTRACK | UDS_SETBUDDYINT, CRect(), this, SPIN_ID);
    // Lấy chiều rộng của điều khiển spin
    CRect rect;
    m_spin.GetWindowRect(&rect);
    m_spin_width = rect.Width();
    // Thiết lập vùng của điều khiển spin (là điều khiển con của CEdit, phần nằm ngoài ClientRect sẽ bị cắt)
    GetClientRect(&rect);
    rect.left = rect.right - m_spin_width;
    m_spin.MoveWindow(&rect);
    // Đặt điều khiển Edit làm điều khiển liên kết (buddy)
    m_spin.SetBuddy(this);
    // Thiết lập phạm vi mặc định
    m_spin.SetRange(0, 999);
    CEdit::PreSubclassWindow();
}


BOOL CSpinEdit::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Thêm mã chuyên biệt và/hoặc gọi lớp cơ sở tại đây

    //Nếu m_mouse_wheel_enable là false thì không phản hồi thông điệp con lăn chuột
    if (pMsg->message == WM_MOUSEWHEEL && !m_mouse_wheel_enable)
    {
        //Gửi thông điệp con lăn chuột tới cửa sổ cha
        CWnd* pParent = GetParent();
        pParent->SendMessage(WM_MOUSEWHEEL, pMsg->wParam, pMsg->lParam);
        return true;
    }

    return CEdit::PreTranslateMessage(pMsg);
}


afx_msg LRESULT CSpinEdit::OnTabletQuerysystemgesturestatus(WPARAM wParam, LPARAM lParam)
{
    return 0;
}


void CSpinEdit::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
    //Ở đây phản hồi sự kiện khi nhấn nút lên/xuống của nút tăng giảm (spin button)
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    int value = GetValue();
    value += pNMUpDown->iDelta * m_step;
    value /= m_step;
    value *= m_step;
    SetValue(value);
    pNMUpDown->iDelta = 0;
    *pResult = 0;
    CWnd* pParent = GetParent();
    if (pParent != nullptr)
        pParent->SendMessage(WM_SPIN_EDIT_POS_CHANGED, (WPARAM)this, (LPARAM)pNMUpDown);
}


void CSpinEdit::OnKillFocus(CWnd* pNewWnd)
{
    CEdit::OnKillFocus(pNewWnd);

    // TODO: Thêm mã xử lý thông điệp tại đây
    CString str;
    GetWindowTextW(str);
    int value = _ttoi(str.GetString());
    SetValue(value);    // m_spin sẽ kiểm tra và giới hạn phạm vi giá trị, đồng thời cập nhật văn bản của điều khiển CEdit
}


void CSpinEdit::OnEnable(BOOL bEnable)
{
    CEdit::OnEnable(bEnable);
    m_spin.EnableWindow(bEnable);
}


void CSpinEdit::OnSize(UINT nType, int cx, int cy)
{
    CEdit::OnSize(nType, cx, cy);
    CRect rect;
    GetClientRect(&rect);
    rect.left = rect.right - m_spin_width;
    m_spin.MoveWindow(&rect);
}
