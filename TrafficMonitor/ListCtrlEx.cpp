#include "stdafx.h"
#include "ListCtrlEx.h"

IMPLEMENT_DYNAMIC(CListCtrlEx, CListCtrl)

CListCtrlEx::CListCtrlEx()
{
}


CListCtrlEx::~CListCtrlEx()
{
}

void CListCtrlEx::Edit(int row, int col)
{
    EnsureVisible(row, FALSE);				//Đảm bảo hàng đang chỉnh sửa luôn hiển thị
    m_editing = true;

    m_edit_row = row;
    m_edit_col = col;
    CRect item_rect;
    GetSubItemRect(row, col, LVIR_LABEL, item_rect);	//Lấy hình chữ nhật của mục con

    CString text = GetItemText(row, col);		//Lấy nội dung của mục con

    m_item_edit.SetWindowText(text);		//Hiển thị nội dung của mục con vào ô chỉnh sửa
    m_item_edit.ShowWindow(SW_SHOW);		//Hiển thị ô chỉnh sửa
    m_item_edit.MoveWindow(item_rect);		//Di chuyển ô chỉnh sửa lên trên mục con, che phủ mục con
    m_item_edit.SetFocus();					//Cho ô chỉnh sửa nhận tiêu điểm
    m_item_edit.SetSel(0, -1);
}

void CListCtrlEx::SetEditColMethod(eEditColMethod method)
{
    m_edit_col_method = method;
}

void CListCtrlEx::SetEditableCol(const std::initializer_list<int>& paras)
{
    m_edit_cols = paras;
}

void CListCtrlEx::EndEdit()
{
    if (m_editing)
    {
        if (m_edit_row >= 0 && m_edit_row < GetItemCount())
        {
            CString str;
            m_item_edit.GetWindowText(str);	//Lấy nội dung của ô chỉnh sửa
            SetItemText(m_edit_row, m_edit_col, str);	//Cập nhật nội dung đó vào CListCtrl
        }
        m_item_edit.ShowWindow(SW_HIDE);//Ẩn ô chỉnh sửa
        m_editing = false;
    }
}

BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
    ON_EN_KILLFOCUS(IDC_ITEM_EDITBOX, &CListCtrlEx::OnEnKillfocusEdit1)
    ON_NOTIFY_REFLECT_EX(NM_DBLCLK, &CListCtrlEx::OnNMDblclk)
    ON_NOTIFY_REFLECT(LVN_BEGINSCROLL, &CListCtrlEx::OnLvnBeginScroll)
    ON_MESSAGE(WM_TABLET_QUERYSYSTEMGESTURESTATUS, &CListCtrlEx::OnTabletQuerysystemgesturestatus)
END_MESSAGE_MAP()


void CListCtrlEx::OnEnKillfocusEdit1()
{
    //Phản hồi khi điều khiển chỉnh sửa văn bản mất tiêu điểm
    EndEdit();
}



void CListCtrlEx::PreSubclassWindow()
{
    // TODO: Thêm mã chuyên biệt và/hoặc gọi lớp cơ sở tại đây
    m_item_edit.Create(WS_BORDER | ES_AUTOHSCROLL, CRect(), this, IDC_ITEM_EDITBOX);
    m_item_edit.SetFont(GetFont());

    CListCtrl::PreSubclassWindow();
}


BOOL CListCtrlEx::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây

    //Nếu cột được nhấp đúp là cột cần chỉnh sửa
    if (m_edit_col_method == EC_ALL
        || (m_edit_col_method == EC_SPECIFIED && m_edit_cols.find(pNMItemActivate->iSubItem) != m_edit_cols.end()))
    {
        Edit(pNMItemActivate->iItem, pNMItemActivate->iSubItem);
        *pResult = 0;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


void CListCtrlEx::OnLvnBeginScroll(NMHDR *pNMHDR, LRESULT *pResult)
{
    // Tính năng này yêu cầu Internet Explorer 5.5 trở lên.
    // Ký hiệu _WIN32_IE phải >= 0x0560.
    LPNMLVSCROLL pStateChanged = reinterpret_cast<LPNMLVSCROLL>(pNMHDR);
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    EndEdit();

    *pResult = 0;
}


afx_msg LRESULT CListCtrlEx::OnTabletQuerysystemgesturestatus(WPARAM wParam, LPARAM lParam)
{
    return 0;
}


BOOL CListCtrlEx::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
        //Khi đang chỉnh sửa, nhấn Enter sẽ kết thúc chỉnh sửa thay vì đóng hộp thoại
        if (m_editing)
        {
            EndEdit();
            return TRUE;
        }
    }

    return CListCtrl::PreTranslateMessage(pMsg);
}
