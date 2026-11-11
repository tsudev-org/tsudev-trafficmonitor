// MainWndColorDlg.cpp : tệp triển khai
//

#include "stdafx.h"
#include "TsudevTrafficMonitor.h"
#include "MainWndColorDlg.h"
#include "afxdialogex.h"
#include "CMFCColorDialogEx.h"
#include "TsudevTrafficMonitorDlg.h"


// Hộp thoại CMainWndColorDlg

IMPLEMENT_DYNAMIC(CMainWndColorDlg, CBaseDialog)

CMainWndColorDlg::CMainWndColorDlg(const std::map<CommonDisplayItem, COLORREF>& colors, CWnd* pParent /*=NULL*/)
	: CBaseDialog(IDD_MAIN_COLOR_DIALOG, pParent), m_colors(colors)
{
}

CMainWndColorDlg::~CMainWndColorDlg()
{
}

CString CMainWndColorDlg::GetDialogName() const
{
    return _T("MainWndColorDlg");
}

void CMainWndColorDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
}


BEGIN_MESSAGE_MAP(CMainWndColorDlg, CBaseDialog)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CMainWndColorDlg::OnNMDblclkList1)
END_MESSAGE_MAP()


// Trình xử lý thông điệp của CMainWndColorDlg


BOOL CMainWndColorDlg::OnInitDialog()
{
	CBaseDialog::OnInitDialog();

	// TODO:  Thêm khởi tạo bổ sung tại đây

    SetIcon(theApp.GetMenuIcon(IDI_MAIN_WINDOW), FALSE);		// Thiết lập biểu tượng nhỏ

    //Khởi tạo điều khiển danh sách
    CRect rect;
    m_list_ctrl.GetClientRect(rect);
    m_list_ctrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
    int width0, width1;
    width0 = rect.Width() * 2 / 3;
    width1 = rect.Width() - width0 - theApp.DPI(20) - 1;
    m_list_ctrl.InsertColumn(0, CCommon::LoadText(IDS_ITEM), LVCFMT_LEFT, width0);		//Chèn cột thứ 0
    m_list_ctrl.InsertColumn(1, CCommon::LoadText(IDS_COLOR), LVCFMT_LEFT, width1);		//Chèn cột thứ 1
    m_list_ctrl.SetDrawItemRangMargin(theApp.DPI(2));

    static std::set<CommonDisplayItem> all_skin_items;
    CTsudevTrafficMonitorDlg::Instance()->GetCurSkin().GetSkinDisplayItems(all_skin_items);

    //Chèn hàng vào danh sách
    for (auto iter = all_skin_items.begin(); iter != all_skin_items.end(); ++iter)
    {
        CString item_name = iter->GetItemName();
        if (!item_name.IsEmpty())
        {
            int index = m_list_ctrl.GetItemCount();
            m_list_ctrl.InsertItem(index, item_name);
            m_list_ctrl.SetItemColor(index, 1, m_colors[*iter]);
            m_list_ctrl.SetItemData(index, (DWORD_PTR)&(*iter));
        }
    }


	return TRUE;  // return TRUE unless you set the focus to a control
				  // Ngoại lệ: trang thuộc tính OCX phải trả về FALSE
}



void CMainWndColorDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    int index = pNMItemActivate->iItem;
    COLORREF color = m_list_ctrl.GetItemColor(index, 1);
    CMFCColorDialogEx colorDlg(color, 0, this);
    if (colorDlg.DoModal() == IDOK)
    {
        color = colorDlg.GetColor();
        m_list_ctrl.SetItemColor(index, 1, color);
        CommonDisplayItem* item = (CommonDisplayItem*)(m_list_ctrl.GetItemData(index));
        m_colors[*item] = color;
    }

    *pResult = 0;
}
