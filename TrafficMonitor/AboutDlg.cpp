#include "stdafx.h"
#include "TrafficMonitor.h"
#include "AboutDlg.h"
#include "MessageDlg.h"
#include "DrawCommon.h"

BEGIN_MESSAGE_MAP(CAboutDlg, CBaseDialog)
    //ON_STN_CLICKED(IDC_STATIC_DONATE, &CAboutDlg::OnStnClickedStaticDonate)
    ON_MESSAGE(WM_LINK_CLICKED, &CAboutDlg::OnLinkClicked)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

CAboutDlg::CAboutDlg() : CBaseDialog(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_MAIL, m_mail);
    DDX_Control(pDX, IDC_STATIC_GITHUB, m_github);
    DDX_Control(pDX, IDC_TRANSLATOR_STATIC, m_translator_static);
    DDX_Control(pDX, IDC_STATIC_LICENSE, m_license);
    DDX_Control(pDX, IDC_OPENHARDWAREMONITOR_LINK, m_openhardwaremonitor_link);
    DDX_Control(pDX, IDC_TINYXML2_LINK, m_tinyxml2_link);
    DDX_Control(pDX, IDC_STATIC_TSUDEV, m_tsudev);
}

CString CAboutDlg::GetDialogName() const
{
    return _T("AboutDlg");
}

bool CAboutDlg::InitializeControls()
{
    RepositionTextBasedControls({
        { CtrlTextInfo::L4, IDC_STATIC_MAIL },
        { CtrlTextInfo::L3, IDC_STATIC_GITHUB },
        { CtrlTextInfo::L2, IDC_STATIC_TSUDEV },
        { CtrlTextInfo::L4, IDC_STATIC_LICENSE }              //Dòng "Giấy phép": mở LICENSE (nơi giữ thông báo bản quyền bắt buộc)
        });
    return true;
}

CRect CAboutDlg::CalculatePicRect()
{
    CRect rect;
    GetClientRect(rect);
    CRect rc_pic = rect;
    ::GetWindowRect(GetDlgItem(IDC_STATIC_VERSION)->GetSafeHwnd(), rect);
    ScreenToClient(rect);
    rc_pic.bottom = rect.top - theApp.DPI(6);
    if (rc_pic.Height() <= 0)
        rc_pic.bottom = rc_pic.top + theApp.DPI(50);
    return rc_pic;
}

BOOL CAboutDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  Thêm khởi tạo bổ sung tại đây
    SetRememberDlgSize(false);

    //Bản Việt hóa: "Liên hệ" mở trang báo lỗi của repo; "GitHub" mở repo; "Giấy phép" hiện nội dung LICENSE
    m_mail.SetURL(CString(FORK_REPO_URL) + _T("/issues"));   //Thiết lập siêu liên kết
    m_github.SetURL(FORK_REPO_URL);
    m_tsudev.SetURL(TSUDEV_HOME_URL);
    m_license.SetLinkIsURL(false);

    m_openhardwaremonitor_link.SetURL(THIRD_PARTY_LIBREHARDWAREMONITOR_URL);
    m_tinyxml2_link.SetURL(THIRD_PARTY_TINYXML2_URL);
    m_openhardwaremonitor_link.SetBackgroundColor(GetSysColor(COLOR_WINDOW));
    m_tinyxml2_link.SetBackgroundColor(GetSysColor(COLOR_WINDOW));
    m_mail.SetBackgroundColor(GetSysColor(COLOR_WINDOW));
    m_github.SetBackgroundColor(GetSysColor(COLOR_WINDOW));
    m_tsudev.SetBackgroundColor(GetSysColor(COLOR_WINDOW));
    m_license.SetBackgroundColor(GetSysColor(COLOR_WINDOW));

    //Thiết lập thông tin phiên bản
    CString version_info;
    GetDlgItemText(IDC_STATIC_VERSION, version_info);
    CString str_lite;
#ifdef WITHOUT_TEMPERATURE
    str_lite = CCommon::LoadText(_T(" ("), IDS_WITHOUT_TEMPERATURE, _T(")"));
#endif
    version_info = CCommon::StringFormat(version_info, { str_lite, VERSION });

#ifdef COMPILE_FOR_WINXP
    version_info += _T(" (For WinXP)");
#endif // COMPILE_FOR_WINXP

#ifdef _M_ARM64EC
    version_info += _T(" (Arm64EC)");
#elif _M_X64
    version_info += _T(" (x64)");
#endif

#ifdef _DEBUG
    version_info += _T(" (Debug)");
#endif

    SetDlgItemText(IDC_STATIC_VERSION, version_info);

    //Thiết lập ngày biên dịch gần nhất
    CString temp_str;
    GetDlgItemText(IDC_STATIC_COPYRIGHT, temp_str);
    CString str_compile_time = CCommon::GetLastCompileTime();
    temp_str = CCommon::StringFormat(temp_str, { COPYRITE_YEAR, str_compile_time });
    SetDlgItemText(IDC_STATIC_COPYRIGHT, temp_str);

    m_tool_tip.Create(this, TTS_ALWAYSTIP | TTS_NOPREFIX);
    m_tool_tip.AddTool(&m_mail, CCommon::LoadText(IDS_SEND_EMAIL_TO_ATHOUR, _T("\r\n")) + m_mail.GetURL());
    m_tool_tip.AddTool(&m_github, CCommon::LoadText(IDS_GOTO_GITHUB, _T("\r\n")) + FORK_REPO_URL);
    m_tool_tip.AddTool(&m_tsudev, TSUDEV_HOME_URL);
    m_tool_tip.AddTool(&m_openhardwaremonitor_link, m_openhardwaremonitor_link.GetURL());
    m_tool_tip.AddTool(&m_tinyxml2_link, m_tinyxml2_link.GetURL());

    m_tool_tip.SetDelayTime(300);   //Thiết lập độ trễ
    m_tool_tip.SetMaxTipWidth(800);

    //Thiết lập thông tin người dịch
    const auto& language_info{ theApp.m_str_table.GetLanguageInfo() };
    wstring language_tag{ language_info.bcp_47 };
    if (language_info.translator.empty())           //Không hiển thị thông tin người dịch khi không có người dịch
    {
        m_translator_static.ShowWindow(SW_HIDE);
    }
    m_translator_static.SetWindowTextW(theApp.m_str_table.LoadTextFormat(L"TXT_ABOUT_TRANSLATOR", { language_info.display_name, language_info.translator }).c_str());
    std::wstring translator_url{ language_info.translator_url };
    if (!translator_url.empty())     //Hiển thị thông tin người dịch
    {
        //Nếu url chứa “@” nhưng phía trước chưa có “mailto:” thì thêm “mailto:” vào đầu
        if (translator_url.find(L'@') != std::wstring::npos && (translator_url.size() < 7 || translator_url.substr(0, 7) != L"mailto:"))
            translator_url = L"mailto:" + translator_url;
        m_translator_static.SetURL(translator_url.c_str());
        CString str_tool_tip = CCommon::LoadText(IDS_CONTACT_TRANSLATOR);
        str_tool_tip += _T("\r\n");
        str_tool_tip += translator_url.c_str();
        m_tool_tip.AddTool(&m_translator_static, str_tool_tip);
    }
    m_translator_static.SetBackgroundColor(GetSysColor(COLOR_WINDOW));

    //Tải ảnh
    m_about_pic.LoadBitmap(IDB_ABOUT_BACKGROUND_HD);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // Ngoại lệ: trang thuộc tính OCX phải trả về FALSE
}

BOOL CAboutDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Thêm mã chuyên biệt và/hoặc gọi lớp cơ sở tại đây
    if (pMsg->message == WM_MOUSEMOVE)
        m_tool_tip.RelayEvent(pMsg);

    return CBaseDialog::PreTranslateMessage(pMsg);
}

//void CAboutDlg::OnStnClickedStaticDonate()
//{
//  CDonateDlg donateDlg;
//  donateDlg.DoModal();
//}

afx_msg LRESULT CAboutDlg::OnLinkClicked(WPARAM wParam, LPARAM lParam)
{
    CWnd* pCtrl = (CWnd*)wParam;
    if (pCtrl == &m_license)
    {
        CMessageDlg dlg;
        dlg.SetWindowTitle(CCommon::LoadText(IDS_LICENSE));
        dlg.SetInfoText(CCommon::LoadText(IDS_LICENSE_EXPLAIN));
        dlg.SetMessageText(CCommon::GetTextResource(IDR_LICENSE, 1));
        dlg.DoModal();
    }

    return 0;
}

void CAboutDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
                       // TODO: Thêm mã xử lý thông điệp tại đây
                       // Không gọi CBaseDialog::OnPaint() cho thông điệp vẽ
    CDrawCommon draw;
    draw.Create(&dc, this);
    CRect rc_pic = CalculatePicRect();
    draw.GetDC()->FillSolidRect(rc_pic, RGB(161, 200, 255));
    draw.DrawBitmap(m_about_pic, rc_pic.TopLeft(), rc_pic.Size(), CDrawCommon::StretchMode::FIT);
}


BOOL CAboutDlg::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây
    CRect draw_rect;
    GetClientRect(draw_rect);
    pDC->FillSolidRect(draw_rect, GetSysColor(COLOR_WINDOW));

    //Vẽ nền trắng
    int white_height;       //Chiều cao vùng màu trắng
    CRect rc_copyright{};
    ::GetWindowRect(GetDlgItem(IDOK)->GetSafeHwnd(), rc_copyright);
    ScreenToClient(rc_copyright);
    white_height = rc_copyright.top - theApp.DPI(6);

    //Vẽ đường phân cách phía trên nút “OK”
    CRect rc_line{ draw_rect };
    rc_line.top = white_height;
    rc_line.bottom = white_height + theApp.DPI(1);
    pDC->FillSolidRect(rc_line, RGB(210, 210, 210));

    //Vẽ nền xám
    CRect rc_gray{ rc_line };
    rc_gray.top = rc_line.bottom;
    rc_gray.bottom = draw_rect.bottom;
    pDC->FillSolidRect(rc_gray, GetSysColor(COLOR_BTNFACE));

    return TRUE;
    //return CBaseDialog::OnEraseBkgnd(pDC);
}


HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CBaseDialog::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  Thay đổi bất kỳ thuộc tính nào của DC tại đây
    //Bỏ nền xám của điều khiển static
    UINT ctrl_id = pWnd->GetDlgCtrlID();
    if (ctrl_id == IDC_STATIC_VERSION || ctrl_id == IDC_STATIC_COPYRIGHT || ctrl_id == IDC_STATIC)
    {
        static HBRUSH hBackBrush{};
        if (hBackBrush == NULL)
            hBackBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
        pDC->SetBkColor(GetSysColor(COLOR_WINDOW));
        return hBackBrush;
    }

    // TODO:  Nếu bút vẽ mặc định không phù hợp thì trả về bút vẽ khác
    return hbr;
}
