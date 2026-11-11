// IconSelectDlg.cpp : tệp triển khai
//

#include "stdafx.h"
#include "TsudevTrafficMonitor.h"
#include "IconSelectDlg.h"
#include "afxdialogex.h"


// Hộp thoại CIconSelectDlg

IMPLEMENT_DYNAMIC(CIconSelectDlg, CBaseDialog)

CIconSelectDlg::CIconSelectDlg(int icon_selected, CWnd* pParent /*=NULL*/)
    : CBaseDialog(IDD_ICON_SELECT_DIALOG, pParent), m_icon_selected{ icon_selected }
{

}

CIconSelectDlg::~CIconSelectDlg()
{
}

int CIconSelectDlg::GetIconSelected() const
{
    if (m_icon_selected < 0 || m_icon_selected >= MAX_NOTIFY_ICON)
        return 0;
    return m_icon_selected;
}

void CIconSelectDlg::SetAutoAdaptNotifyIcon(bool val)
{
    m_atuo_adapt_notify_icon = val;
}

bool CIconSelectDlg::AutoAdaptNotifyIcon() const
{
    return m_atuo_adapt_notify_icon;
}

void CIconSelectDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_ICON_PREVIEW, m_preview_pic);
    DDX_Control(pDX, IDC_COMBO1, m_icon_select_combo);
    DDX_Control(pDX, IDC_AUTO_ADAPT_CHECK, m_auto_adapt_chk);
}

void CIconSelectDlg::DrawPreviewIcon(CDC* pDC)
{
    //pDC->FillSolidRect(CRect(CPoint(ICON_X, ICON_Y), CSize(theApp.DPI(16), theApp.DPI(16))), RGB(0, 0, 0));
    //pDC->DrawIcon(ICON_X, ICON_Y, m_icons[m_icon_selected]);
    ::DrawIconEx(pDC->m_hDC, ICON_X, ICON_Y, theApp.m_notify_icons[GetIconSelected()], theApp.DPI(16), theApp.DPI(16), 0, NULL, DI_NORMAL);
}


BEGIN_MESSAGE_MAP(CIconSelectDlg, CBaseDialog)
    //ON_WM_TIMER()
    ON_CBN_SELCHANGE(IDC_COMBO1, &CIconSelectDlg::OnCbnSelchangeCombo1)
    ON_MESSAGE(WM_CONTROL_REPAINT, &CIconSelectDlg::OnControlRepaint)
    ON_BN_CLICKED(IDC_AUTO_ADAPT_CHECK, &CIconSelectDlg::OnBnClickedAutoAdaptCheck)
END_MESSAGE_MAP()


// Trình xử lý thông điệp của CIconSelectDlg


BOOL CIconSelectDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  Thêm khởi tạo bổ sung tại đây
    SetIcon(theApp.GetMenuIcon(IDI_NOTIFY), FALSE);		// Thiết lập biểu tượng nhỏ

    //Thiết lập kích thước ảnh xem trước
    m_preview_pic.SetWindowPos(nullptr, 0, 0, PREVIEW_WIDTH, PREVIEW_HEIGHT, SWP_NOZORDER | SWP_NOMOVE);
    if (m_icon_selected == 4 || m_icon_selected == 5)
        m_preview_pic.SetPicture((HBITMAP)LoadImage(AfxGetInstanceHandle(),
            MAKEINTRESOURCE(IDB_NOTIFY_ICON_PREVIEW_LIGHT), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION));
    else
        m_preview_pic.SetPicture((HBITMAP)LoadImage(AfxGetInstanceHandle(),
            MAKEINTRESOURCE(IDB_NOTIFY_ICON_PREVIEW), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION));

    //Khởi tạo danh sách thả xuống
    m_icon_select_combo.AddString(CCommon::LoadText(IDS_DEFAULT_ICON));
    m_icon_select_combo.AddString(CCommon::LoadText(IDS_ICON, _T(" 1")));
    m_icon_select_combo.AddString(CCommon::LoadText(IDS_ICON, _T(" 2")));
    m_icon_select_combo.AddString(CCommon::LoadText(IDS_ICON, _T(" 3")));
    m_icon_select_combo.AddString(CCommon::LoadText(IDS_ICON, _T(" 4")));
    m_icon_select_combo.AddString(CCommon::LoadText(IDS_ICON, _T(" 5")));
    m_icon_select_combo.SetCurSel(m_icon_selected);

    m_auto_adapt_chk.SetCheck(m_atuo_adapt_notify_icon);
    m_auto_adapt_chk.EnableWindow(theApp.m_win_version.GetMajorVersion() >= 10);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // Ngoại lệ: trang thuộc tính OCX phải trả về FALSE
}


void CIconSelectDlg::OnCbnSelchangeCombo1()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    m_icon_selected = m_icon_select_combo.GetCurSel();
    if (m_icon_selected == 4 || m_icon_selected == 5)
        m_preview_pic.SetPicture((HBITMAP)LoadImage(AfxGetInstanceHandle(),
            MAKEINTRESOURCE(IDB_NOTIFY_ICON_PREVIEW_LIGHT), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION));
    else
        m_preview_pic.SetPicture((HBITMAP)LoadImage(AfxGetInstanceHandle(),
            MAKEINTRESOURCE(IDB_NOTIFY_ICON_PREVIEW), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION));
    DrawPreviewIcon(m_preview_pic.GetDC());
}


afx_msg LRESULT CIconSelectDlg::OnControlRepaint(WPARAM wParam, LPARAM lParam)
{
    CWnd* pControl = (CWnd*)wParam;
    CDC* pDC = (CDC*)lParam;
    if (pControl == &m_preview_pic)
    {
        //Khi nhận thông điệp vẽ lại của điều khiển m_preview_pic thì vẽ lại biểu tượng đồng thời
        DrawPreviewIcon(pDC);
    }
    return 0;
}


void CIconSelectDlg::OnBnClickedAutoAdaptCheck()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    m_atuo_adapt_notify_icon = (m_auto_adapt_chk.GetCheck() != 0);
}

CString CIconSelectDlg::GetDialogName() const
{
    return _T("IconSelectDlg");
}

bool CIconSelectDlg::InitializeControls()
{
    RepositionTextBasedControls({
        { CtrlTextInfo::L1, IDC_SELECT_ICON_STATIC },
        { CtrlTextInfo::C0, IDC_COMBO1 }
    });
    return true;
}
