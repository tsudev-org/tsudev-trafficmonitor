// SkinDlg.cpp : tệp triển khai
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "SkinDlg.h"
#include "afxdialogex.h"
#include "SkinAutoAdaptSettingDlg.h"
#include "SkinManager.h"


// Hộp thoại CSkinDlg

IMPLEMENT_DYNAMIC(CSkinDlg, CBaseDialog)

CSkinDlg::CSkinDlg(CWnd* pParent /*=NULL*/)
    : CBaseDialog(IDD_SKIN_DIALOG, pParent)
{

}

CSkinDlg::~CSkinDlg()
{
}

CString CSkinDlg::GetDialogName() const
{
    return _T("SkinDlg");
}

bool CSkinDlg::InitializeControls()
{
    RepositionTextBasedControls({
        { CtrlTextInfo::L4, IDC_SKIN_AUTO_ADAPT_CHECK, CtrlTextInfo::W32 },
        { CtrlTextInfo::L3, IDC_SKIN_AUTO_ADAPT_BUTTON, CtrlTextInfo::W16 }
    });
    RepositionTextBasedControls({
        { CtrlTextInfo::L4, IDC_SKIN_DOWNLOAD_STATIC },
        { CtrlTextInfo::L3, IDC_SKIN_COURSE_STATIC },
        { CtrlTextInfo::L2, IDC_OPEN_SKIN_DIR_STATIC }
    });

    return true;
}

void CSkinDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SKIN_COURSE_STATIC, m_skin_course);
    DDX_Control(pDX, IDC_SKIN_DOWNLOAD_STATIC, m_skin_download);
    DDX_Control(pDX, IDC_PREVIEW_GROUP_STATIC, m_preview_static);
    DDX_Control(pDX, IDC_NOTIFY_STATIC, m_notify_static);
    DDX_Control(pDX, IDC_OPEN_SKIN_DIR_STATIC, m_open_skin_dir_lnk);
    DDX_Control(pDX, IDC_LIST1, m_skin_list_box);
}


void CSkinDlg::ShowPreview()
{
    //Tải dữ liệu bố cục
    m_skin_data.Load(CSkinManager::Instance().GetSkinName(m_skin_selected));
    //Lấy phông chữ do người dùng thay đổi trong skin hiện tại
    SkinSettingData cur_skin_data;
    if (CSkinManager::Instance().GetSkinSettingDataByIndex(m_skin_selected, cur_skin_data))
    {
        m_skin_data.SetSettingData(cur_skin_data);
    }
    //Lấy kích thước vùng xem trước
    m_view->SetSize(m_skin_data.GetPreviewInfo().width, m_skin_data.GetPreviewInfo().height);
    //Làm mới ảnh xem trước
    m_view->Invalidate();

    //Hiển thị tác giả skin
    SetDlgItemText(IDC_SKIN_INFO, CCommon::LoadText(IDS_SKIN_AUTHOUR, (L" " + m_skin_data.GetSkinInfo().skin_author).c_str()));
    ////Thiết lập thông tin gợi ý
    //bool cover_font_setting{ !m_skin_data.GetSkinInfo().font_info.name.IsEmpty() || (m_skin_data.GetSkinInfo().font_info.size >= MIN_FONT_SIZE && m_skin_data.GetSkinInfo().font_info.size <= MAX_FONT_SIZE) };
    //bool cover_str_setting{ !m_skin_data.GetSkinInfo().display_text.IsInvalid() };
    //cover_font_setting = cover_font_setting && theApp.m_general_data.allow_skin_cover_font;
    //cover_str_setting = cover_str_setting && theApp.m_general_data.allow_skin_cover_text;
    //if (cover_font_setting && cover_str_setting)
    //    m_notify_static.SetWindowTextEx(CCommon::LoadText(IDS_OVERWRITE_FONT_TEXT_WARNING));
    //else if (cover_font_setting)
    //    m_notify_static.SetWindowTextEx(CCommon::LoadText(IDS_OVERWRITE_FONT_WARNING));
    //else if (cover_str_setting)
    //    m_notify_static.SetWindowTextEx(CCommon::LoadText(IDS_OVERWRITE_TEXT_WARNING));
    //else
    //m_notify_static.SetWindowTextEx(_T(""));
}


CRect CSkinDlg::CalculateViewRect()
{
    CRect rect;
    m_preview_static.GetWindowRect(rect);		//Lấy vị trí của group box “Xem trước”
    ScreenToClient(&rect);
    CRect scroll_view_rect{ rect };
    scroll_view_rect.DeflateRect(DPI(12), DPI(40));
    scroll_view_rect.top = rect.top + DPI(28);
    return scroll_view_rect;
}


BEGIN_MESSAGE_MAP(CSkinDlg, CBaseDialog)
    ON_LBN_SELCHANGE(IDC_LIST1, &CSkinDlg::OnLbnSelchangeList1)
    ON_WM_SIZE()
    ON_MESSAGE(WM_LINK_CLICKED, &CSkinDlg::OnLinkClicked)
    ON_BN_CLICKED(IDC_SKIN_AUTO_ADAPT_BUTTON, &CSkinDlg::OnBnClickedSkinAutoAdaptButton)
    ON_BN_CLICKED(IDC_SKIN_AUTO_ADAPT_CHECK, &CSkinDlg::OnBnClickedSkinAutoAdaptCheck)
END_MESSAGE_MAP()


// Trình xử lý thông điệp của CSkinDlg

BOOL CSkinDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  Thêm khởi tạo bổ sung tại đây
    SetIcon(theApp.GetMenuIcon(IDI_SKIN), FALSE);		// Thiết lập biểu tượng nhỏ
    //Khởi tạo hộp chọn
    m_skin_list_box.SetItemHeight(0, DPI(18));
    for (const auto& skin_name : CSkinManager::Instance().GetSkinNames())
    {
        m_skin_list_box.AddString(skin_name.c_str());
    }
    m_skin_list_box.SetCurSel(m_skin_selected);
    //Khởi tạo view xem trước
    m_view = (CSkinPreviewView*)RUNTIME_CLASS(CSkinPreviewView)->CreateObject();
    m_view->Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, CalculateViewRect(), this, 3000);
    m_view->InitialUpdate();
    m_view->SetSkinData(&m_skin_data);
    m_view->ShowWindow(SW_SHOW);

    //Thiết lập thông tin gợi ý
    m_notify_static.SetTextColor(RGB(252, 128, 45));
    m_notify_static.SetBackColor(GetSysColor(COLOR_BTNFACE));
    m_notify_static.SetWindowTextEx(_T(""));

    //Hiển thị ảnh xem trước
    ShowPreview();

    //Khởi tạo điều khiển
    CheckDlgButton(IDC_SKIN_AUTO_ADAPT_CHECK, theApp.m_cfg_data.skin_auto_adapt);
    EnableDlgCtrl(IDC_SKIN_AUTO_ADAPT_BUTTON, theApp.m_cfg_data.skin_auto_adapt);

    //Thiết lập siêu liên kết
    //Bản Việt hóa: hướng dẫn tạo skin bằng tiếng Việt trong repo TrafficMonitor-VN; kho skin vẫn dùng của tác giả gốc (GitHub)
    m_skin_course.SetURL(FORK_SKIN_TUTORIAL_URL);
    m_skin_download.SetURL(FORK_SKIN_DOWNLOAD_URL);
    m_open_skin_dir_lnk.SetLinkIsURL(false);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // Ngoại lệ: trang thuộc tính OCX phải trả về FALSE
}


void CSkinDlg::OnLbnSelchangeList1()
{
    m_skin_selected = m_skin_list_box.GetCurSel();
    ShowPreview();
}


void CSkinDlg::OnSize(UINT nType, int cx, int cy)
{
    CBaseDialog::OnSize(nType, cx, cy);

    // TODO: Thêm mã xử lý thông điệp tại đây
    if (m_preview_static.m_hWnd != NULL && nType != SIZE_MINIMIZED && m_view != nullptr)
        m_view->MoveWindow(CalculateViewRect());
}


afx_msg LRESULT CSkinDlg::OnLinkClicked(WPARAM wParam, LPARAM lParam)
{
    CWnd* pCtrl = (CWnd*)wParam;
    //Đã nhấn “Mở thư mục skin”
    if (pCtrl == &m_open_skin_dir_lnk)
    {
        CreateDirectory(theApp.m_skin_path.c_str(), NULL);       //Nếu thư mục skin chưa tồn tại thì tạo mới
        ShellExecute(NULL, _T("open"), _T("explorer"), theApp.m_skin_path.c_str(), NULL, SW_SHOWNORMAL);
    }
    return 0;
}


void CSkinDlg::OnBnClickedSkinAutoAdaptButton()
{
    CSkinAutoAdaptSettingDlg dlg;
    const auto& skins{ CSkinManager::Instance().GetSkinNames() };
    if (dlg.DoModal() == IDOK)
    {
        int dark_mode_skin = dlg.GetDarkModeSkin();
        int light_mode_skin = dlg.GetLightModeSkin();
        if (dark_mode_skin >= 0 && dark_mode_skin < static_cast<int>(skins.size()))
            theApp.m_cfg_data.skin_name_dark_mode = skins[dark_mode_skin];
        if (light_mode_skin >= 0 && light_mode_skin < static_cast<int>(skins.size()))
            theApp.m_cfg_data.skin_name_light_mode = skins[light_mode_skin];
    }
}


void CSkinDlg::OnOK()
{
    theApp.m_cfg_data.skin_auto_adapt = (IsDlgButtonChecked(IDC_SKIN_AUTO_ADAPT_CHECK) != FALSE);

    CBaseDialog::OnOK();
}


void CSkinDlg::OnBnClickedSkinAutoAdaptCheck()
{
    bool skin_auto_adapt_checked = (IsDlgButtonChecked(IDC_SKIN_AUTO_ADAPT_CHECK) != FALSE);
    EnableDlgCtrl(IDC_SKIN_AUTO_ADAPT_BUTTON, skin_auto_adapt_checked);
}
