// OptionsDlg.cpp: tệp triển khai
//

#include "pch.h"
#include "PluginDemo.h"
#include "OptionsDlg.h"
#include "afxdialogex.h"


// Hộp thoại COptionsDlg

IMPLEMENT_DYNAMIC(COptionsDlg, CDialog)

COptionsDlg::COptionsDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_OPTIONS_DIALOG, pParent)
{

}

COptionsDlg::~COptionsDlg()
{
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
    ON_BN_CLICKED(IDC_SHOW_SECOND_CHECK, &COptionsDlg::OnBnClickedShowSecondCheck)
END_MESSAGE_MAP()


// Trình xử lý thông điệp của COptionsDlg


BOOL COptionsDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  Thêm khởi tạo bổ sung tại đây
    //Khởi tạo trạng thái điều khiển
    CheckDlgButton(IDC_SHOW_SECOND_CHECK, m_data.show_second);
    //CheckDlgButton(IDC_SHOW_LABEL_CHECK, m_data.show_label_text);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // Ngoại lệ: trang thuộc tính OCX phải trả về FALSE
}


void COptionsDlg::OnBnClickedShowSecondCheck()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    m_data.show_second = (IsDlgButtonChecked(IDC_SHOW_SECOND_CHECK) != 0);
}
