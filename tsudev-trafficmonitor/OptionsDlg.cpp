// OptionsDlg.cpp : tệp triển khai
//

#include "stdafx.h"
#include "TsudevTrafficMonitor.h"
#include "OptionsDlg.h"
#include "afxdialogex.h"


// Hộp thoại COptionsDlg

IMPLEMENT_DYNAMIC(COptionsDlg, CBaseDialog)

COptionsDlg::COptionsDlg(int tab, CWnd* pParent /*=NULL*/)
    : CBaseDialog(IDD_OPTIONS_DIALOG, pParent), m_tab_selected{ tab }
{

}

COptionsDlg::~COptionsDlg()
{
}

CString COptionsDlg::GetDialogName() const
{
    return OPTION_DLG_NAME;
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TAB1, m_tab);
}


BEGIN_MESSAGE_MAP(COptionsDlg, CBaseDialog)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_APPLY_BUTTON, &COptionsDlg::OnBnClickedApplyButton)
END_MESSAGE_MAP()


// Trình xử lý thông điệp của COptionsDlg


BOOL COptionsDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  Thêm khởi tạo bổ sung tại đây
    SetIcon(theApp.GetMenuIcon(IDI_SETTINGS), FALSE);       // Thiết lập biểu tượng nhỏ

    //Tạo hộp thoại con
    m_tab1_dlg.Create(IDD_MAIN_WND_SETTINGS_DIALOG, &m_tab);
    m_tab2_dlg.Create(IDD_TASKBAR_SETTINGS_DIALOG, &m_tab);
    m_tab3_dlg.Create(IDD_GENERAL_SETTINGS_DIALOG, &m_tab);

    //Lưu hộp thoại con
    m_tab_vect.push_back(&m_tab1_dlg);
    m_tab_vect.push_back(&m_tab2_dlg);
    m_tab_vect.push_back(&m_tab3_dlg);

    //Lấy chiều cao ban đầu của hộp thoại con
    for (const auto* pDlg : m_tab_vect)
    {
        CRect rect;
        pDlg->GetWindowRect(rect);
        m_tab_height.push_back(rect.Height());
    }

    //Thêm hộp thoại
    m_tab.AddWindow(&m_tab1_dlg, CCommon::LoadText(IDS_MAIN_WINDOW_SETTINGS));
    m_tab.AddWindow(&m_tab2_dlg, CCommon::LoadText(IDS_TASKBAR_WINDOW_SETTINGS));
    m_tab.AddWindow(&m_tab3_dlg, CCommon::LoadText(IDS_GENERAL_SETTINGS));

    //Thêm biểu tượng cho từng thẻ
    CImageList ImageList;
    ImageList.Create(theApp.DPI(16), theApp.DPI(16), ILC_COLOR32 | ILC_MASK, 2, 2);
    ImageList.Add(theApp.GetMenuIcon(IDI_MAIN_WINDOW));
    ImageList.Add(theApp.GetMenuIcon(IDI_TASKBAR_WINDOW));
    ImageList.Add(theApp.GetMenuIcon(IDI_SETTINGS));
    m_tab.SetImageList(&ImageList);
    ImageList.Detach();

    m_tab.SetItemSize(CSize(theApp.DPI(60), theApp.DPI(24)));
    m_tab.AdjustTabWindowSize();

    //Thiết lập thông tin cuộn cho từng cửa sổ con
    for (size_t i = 0; i < m_tab_vect.size(); i++)
    {
        m_tab_vect[i]->SetScrollbarInfo(m_tab.m_tab_rect.Height(), m_tab_height[i]);
    }

    //Thiết lập thẻ được chọn mặc định
    if (m_tab_selected < 0 || m_tab_selected >= m_tab.GetItemCount())
        m_tab_selected = 0;
    m_tab.SetCurTab(m_tab_selected);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // Ngoại lệ: trang thuộc tính OCX phải trả về FALSE
}


void COptionsDlg::OnOK()
{
    // TODO: Thêm mã chuyên biệt và/hoặc gọi lớp cơ sở tại đây
    m_tab1_dlg.OnOK();
    m_tab2_dlg.OnOK();
    m_tab3_dlg.OnOK();

    CBaseDialog::OnOK();
}


void COptionsDlg::OnSize(UINT nType, int cx, int cy)
{
    CBaseDialog::OnSize(nType, cx, cy);

    // TODO: Thêm mã xử lý thông điệp tại đây
    if (nType != SIZE_MINIMIZED)
    {
        //Thiết lập thông tin cuộn cho từng cửa sổ con
        for (size_t i = 0; i < m_tab_vect.size(); i++)
        {
            m_tab_vect[i]->SetScrollbarInfo(m_tab.m_tab_rect.Height(), m_tab_height[i]);
        }
    }
}


void COptionsDlg::OnCancel()
{
    // TODO: Thêm mã chuyên biệt và/hoặc gọi lớp cơ sở tại đây
    m_tab3_dlg.OnCancel();

    CBaseDialog::OnCancel();
}


void COptionsDlg::OnBnClickedApplyButton()
{
    m_tab2_dlg.SaveColorSettingToDefaultStyle();
    ::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_SETTINGS_APPLIED, (WPARAM)this, 0);
    for (size_t i = 0; i < m_tab_vect.size(); i++)
    {
        m_tab_vect[i]->OnSettingsApplied();
    }
}
