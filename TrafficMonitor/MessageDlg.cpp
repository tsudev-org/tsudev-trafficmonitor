// HelpDlg.cpp : tệp triển khai
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "MessageDlg.h"
#include "DrawCommon.h"


// Hộp thoại CMessageDlg

#define MESSAGE_DLG_ICON_SIZE (theApp.DPI(32))
#define MESSAGE_DLG_ICON_VERTIAL_MARGIN (theApp.DPI(12))

IMPLEMENT_DYNAMIC(CMessageDlg, CBaseDialog)

CMessageDlg::CMessageDlg(CWnd* pParent /*=NULL*/)
	: CBaseDialog(IDD_MESSAGE_DIALOG, pParent)
{

}

CMessageDlg::~CMessageDlg()
{
}

void CMessageDlg::SetWindowTitle(LPCTSTR str)
{
	m_title = str;
}

void CMessageDlg::SetInfoText(LPCTSTR str)
{
	m_info = str;
}

void CMessageDlg::SetMessageText(LPCTSTR str)
{
	m_message = str;
}

//void CMessageDlg::SetLinkInfo(LPCTSTR text, LPCTSTR url)
//{
//	m_link_text = text;
//	m_link_url = url;
//}

void CMessageDlg::SetMessageIcon(HICON hIcon)
{
    m_icon = hIcon;
}

void CMessageDlg::SetStandarnMessageIcon(StandardIcon standard_icon)
{
	HICON hIcon;
	PCWSTR icon{};
	switch (standard_icon)
	{
	case SI_INFORMATION:
		icon = IDI_INFORMATION;
		break;
	case SI_WARNING:
		icon = IDI_WARNING;
		break;
	case SI_ERROR:
		icon = IDI_ERROR;
		break;
	}
	HRESULT hr = LoadIconWithScaleDown(NULL, icon, theApp.DPI(32), theApp.DPI(32), &hIcon);
	if (SUCCEEDED(hr))
		SetMessageIcon(hIcon);
}

void CMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HELP_EDIT, m_message_edit);
	DDX_Control(pDX, IDC_INFO_STATIC, m_info_static);
}


BEGIN_MESSAGE_MAP(CMessageDlg, CBaseDialog)
	ON_WM_GETMINMAXINFO()
	//ON_NOTIFY(NM_CLICK, IDC_SYSLINK1, &CMessageDlg::OnNMClickSyslink1)
    ON_WM_PAINT()
    ON_WM_SIZE()
END_MESSAGE_MAP()


// Trình xử lý thông điệp của CMessageDlg


BOOL CMessageDlg::OnInitDialog()
{
	CBaseDialog::OnInitDialog();

	// TODO:  Thêm khởi tạo bổ sung tại đây

	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);		// Thiết lập biểu tượng nhỏ

	//Lấy kích thước ban đầu của cửa sổ
	CRect rect;
	GetWindowRect(rect);
	m_min_size.cx = rect.Width();
	m_min_size.cy = rect.Height();

	SetWindowText(m_title);
	m_info_static.SetWindowText(m_info);
	m_message_edit.SetWindowText(m_message);

	//CWnd* pLinkCtrl = GetDlgItem(IDC_SYSLINK1);
	//if (pLinkCtrl != nullptr)
	//{
	//	pLinkCtrl->ShowWindow(m_show_link_ctrl);
	//	pLinkCtrl->SetWindowText(_T("<a>") + m_link_text + _T("</a>"));
	//}

    //Thiết lập vị trí của biểu tượng
    if (m_icon != NULL)
    {
		CRect rc_info = GetControlRect(&m_info_static);
		//Thiết lập vị trí ngang của điều khiển Static, chừa chỗ cho biểu tượng
		rc_info.left = rc_info.left + MESSAGE_DLG_ICON_SIZE + theApp.DPI(8);
		//Thiết lập vị trí dọc của điều khiển Static
		rc_info.MoveToY(rc_info.top + MESSAGE_DLG_ICON_VERTIAL_MARGIN);
		m_info_static.MoveWindow(rc_info);

		CRect rc_edit = GetControlRect(&m_message_edit);
		rc_edit.top += (MESSAGE_DLG_ICON_VERTIAL_MARGIN * 2);
		m_icon_pos.x = rc_edit.left;
		m_icon_pos.y = (rc_edit.top - MESSAGE_DLG_ICON_SIZE) / 2;
		m_message_edit.MoveWindow(rc_edit);
    }

	//Khi không có biểu tượng và tiêu đề thông điệp rỗng thì ẩn điều khiển Static
	if (m_icon == NULL && m_info.IsEmpty())
	{
		CRect rc_info = GetControlRect(&m_info_static);
		CRect rc_edit = GetControlRect(&m_message_edit);
		rc_edit.top = rc_info.top;
		m_message_edit.MoveWindow(rc_edit);
		m_info_static.ShowWindow(SW_HIDE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // Ngoại lệ: trang thuộc tính OCX phải trả về FALSE
}


void CMessageDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây
	//Giới hạn kích thước tối thiểu của cửa sổ
	lpMMI->ptMinTrackSize.x = m_min_size.cx;		//Thiết lập chiều rộng tối thiểu
	lpMMI->ptMinTrackSize.y = m_min_size.cy;		//Thiết lập chiều cao tối thiểu

	CBaseDialog::OnGetMinMaxInfo(lpMMI);
}


//void CMessageDlg::OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: Thêm mã xử lý thông báo của điều khiển tại đây
//	if(!m_link_url.IsEmpty())
//		ShellExecute(NULL, _T("open"), m_link_url, NULL, NULL, SW_SHOW);	//Mở siêu liên kết
//
//	*pResult = 0;
//}


void CMessageDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
                       // TODO: Thêm mã xử lý thông điệp tại đây
                       // Không gọi CBaseDialog::OnPaint() cho thông điệp vẽ

    CDrawCommon draw;
    draw.Create(&dc, this);
    draw.DrawIcon(m_icon, m_icon_pos, CSize(MESSAGE_DLG_ICON_SIZE, MESSAGE_DLG_ICON_SIZE));
}


void CMessageDlg::OnSize(UINT nType, int cx, int cy)
{
    CBaseDialog::OnSize(nType, cx, cy);

	if (m_info_static.GetSafeHwnd() != NULL && m_icon != NULL)
	{
		CRect rc_info = GetControlRect(&m_info_static);
		rc_info.right = cx;
		m_info_static.MoveWindow(rc_info);
	}
	if (m_message_edit.GetSafeHwnd() != NULL)
	{
		CRect rc_edit = GetControlRect(&m_message_edit);
		//Thiết lập vị trí dọc của điều khiển Edit
		CRect rc_ok = GetControlRect(IDOK);
		if (!rc_ok.IsRectEmpty())
		{
			rc_edit.bottom = rc_ok.top - theApp.DPI(6);
			m_message_edit.MoveWindow(rc_edit);
		}
	}
}
