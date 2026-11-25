// SignInDlg.cpp: màn đăng nhập tài khoản tsudev và màn TsuDEV Pro/Ủng hộ (AUTH_AND_ACCOUNT 13.2, 13.4; QR1)
//

#include "stdafx.h"
#include "TsuTramon.h"
#include "SignInDlg.h"
#include <commctrl.h>

IMPLEMENT_DYNAMIC(CSignInDlg, CBaseDialog)

CSignInDlg::CSignInDlg(Mode mode, CWnd* pParent /*=NULL*/)
    : CBaseDialog(IDD_SIGN_IN_DIALOG, pParent), m_mode(mode)
{
    SetRememberDlgSize(false);
}

CSignInDlg::~CSignInDlg()
{
}

void CSignInDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SIGNIN_TERMS_LINK, m_terms_link);
    DDX_Control(pDX, IDC_SIGNIN_PRIVACY_LINK, m_privacy_link);
}

BEGIN_MESSAGE_MAP(CSignInDlg, CBaseDialog)
    ON_WM_DESTROY()
    ON_WM_CTLCOLOR()
    ON_BN_CLICKED(IDC_SIGNIN_BUTTON1, &CSignInDlg::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_SIGNIN_BUTTON2, &CSignInDlg::OnBnClickedButton2)
    ON_BN_CLICKED(IDC_SIGNIN_BUTTON3, &CSignInDlg::OnBnClickedButton3)
    ON_BN_CLICKED(IDC_SIGNIN_PASSKEY_BUTTON, &CSignInDlg::OnBnClickedPasskey)
    ON_BN_CLICKED(IDC_SIGNIN_QR_BUTTON, &CSignInDlg::OnBnClickedQr)
    ON_BN_CLICKED(IDC_SIGNIN_GUEST_BUTTON, &CSignInDlg::OnBnClickedGuest)
END_MESSAGE_MAP()

CString CSignInDlg::ProStatusText(const CProAccount::Status& s, int64_t now)
{
    if (s.has_entitlement && s.entitlement.exp > now)
    {
        if (s.entitlement.pro_until > now)
            return CCommon::LoadTextFormat(IDS_ACCOUNT_PRO_ACTIVE, { theApp.FormatProDate(s.entitlement.pro_until) });
        if (s.entitlement.pro_until > 0)
            return CCommon::LoadTextFormat(IDS_ACCOUNT_PRO_EXPIRED, { theApp.FormatProDate(s.entitlement.pro_until) });
        return CCommon::LoadText(IDS_ACCOUNT_PRO_NONE);
    }
    if (s.signed_in)
        return CCommon::LoadText(IDS_ACCOUNT_PRO_UNKNOWN);
    return CString();
}

BOOL CSignInDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    //Phông: tiêu đề 24-28 px đậm (13.2), ghi chú nhỏ hơn chữ thường
    LOGFONT lf{};
    GetFont()->GetLogFont(&lf);
    LOGFONT title = lf;
    title.lfHeight = -DPI(24);
    title.lfWeight = FW_BOLD;
    m_title_font.CreateFontIndirect(&title);
    LOGFONT small_lf = lf;
    small_lf.lfHeight = lf.lfHeight * 9 / 10;
    m_small_font.CreateFontIndirect(&small_lf);
    GetDlgItem(IDC_SIGNIN_TITLE_STATIC)->SetFont(&m_title_font);
    GetDlgItem(IDC_SIGNIN_LAST_STATIC)->SetFont(&m_small_font);
    GetDlgItem(IDC_SIGNIN_GUEST_NOTE_STATIC)->SetFont(&m_small_font);

    //Logo sản phẩm 48 px (BRAND_ASSETS mục 2)
    const int logo = DPI(48);
    m_logo = static_cast<HICON>(LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, logo, logo, LR_DEFAULTCOLOR));
    if (m_logo != NULL)
        static_cast<CStatic*>(GetDlgItem(IDC_SIGNIN_LOGO_STATIC))->SetIcon(m_logo);

    m_terms_link.SetURL(FORK_TSUDEV_TERMS_URL);
    m_privacy_link.SetURL(FORK_TSUDEV_PRIVACY_URL);
    SetDlgItemText(IDC_SIGNIN_TERMS_LINK, CCommon::LoadText(IDS_SIGNIN_TERMS_LINK));
    SetDlgItemText(IDC_SIGNIN_PRIVACY_LINK, CCommon::LoadText(IDS_SIGNIN_PRIVACY_LINK));

    if (m_mode == Mode::SignIn)
    {
        SetWindowText(CCommon::LoadText(IDS_SIGNIN_CAPTION));
        SetDlgItemText(IDC_SIGNIN_TITLE_STATIC, CCommon::LoadText(IDS_SIGNIN_WELCOME));
        SetDlgItemText(IDC_SIGNIN_VALUE_STATIC, CCommon::LoadText(IDS_SIGNIN_VALUE));
        SetDlgItemText(IDC_SIGNIN_BUTTON1, CCommon::LoadText(IDS_SIGNIN_GOOGLE));
        SetDlgItemText(IDC_SIGNIN_BUTTON2, CCommon::LoadText(IDS_SIGNIN_GITHUB));
        SetDlgItemText(IDC_SIGNIN_BUTTON3, CCommon::LoadText(IDS_SIGNIN_EMAIL));
        SetDlgItemText(IDC_SIGNIN_LAST_STATIC, CCommon::LoadText(IDS_SIGNIN_LAST_USED));
        SetDlgItemText(IDC_SIGNIN_TERMS_CHECK, CCommon::LoadText(IDS_SIGNIN_TERMS_CHECK));
        SetDlgItemText(IDC_SIGNIN_MORE_STATIC, CCommon::LoadText(IDS_SIGNIN_MORE));
        SetDlgItemText(IDC_SIGNIN_PASSKEY_BUTTON, CCommon::LoadText(IDS_SIGNIN_PASSKEY));
        SetDlgItemText(IDC_SIGNIN_QR_BUTTON, CCommon::LoadText(IDS_SIGNIN_QR));
        SetDlgItemText(IDC_SIGNIN_OR_STATIC, CCommon::LoadText(IDS_SIGNIN_OR));
        SetDlgItemText(IDC_SIGNIN_GUEST_BUTTON, CCommon::LoadText(IDS_SIGNIN_GUEST));
        SetDlgItemText(IDC_SIGNIN_GUEST_NOTE_STATIC, CCommon::LoadText(IDS_SIGNIN_GUEST_NOTE));
        CheckDlgButton(IDC_SIGNIN_TERMS_CHECK, BST_UNCHECKED);      //13.4: KHÔNG tích sẵn
        ShowDlgCtrl(IDC_SIGNIN_STATUS_STATIC, false);
    }
    else
    {
        const CProAccount::Status s = theApp.m_pro_account.GetStatus();
        SetWindowText(CCommon::LoadText(IDS_PRO_SCREEN_CAPTION));
        SetDlgItemText(IDC_SIGNIN_TITLE_STATIC, CCommon::LoadText(IDS_PRO_SCREEN_TITLE));
        SetDlgItemText(IDC_SIGNIN_VALUE_STATIC, CCommon::LoadText(IDS_PRO_SCREEN_VALUE));
        SetDlgItemText(IDC_SIGNIN_STATUS_STATIC, ProStatusText(s, static_cast<int64_t>(_time64(nullptr))));
        SetDlgItemText(IDC_SIGNIN_BUTTON1, CCommon::LoadText(IDS_PRO_SCREEN_BUY));
        SetDlgItemText(IDC_SIGNIN_BUTTON2, CCommon::LoadText(IDS_PRO_SCREEN_DONATE));
        SetDlgItemText(IDC_SIGNIN_BUTTON3, CCommon::LoadText(IDS_PRO_SCREEN_SIGN_IN));
        SetDlgItemText(IDC_SIGNIN_GUEST_BUTTON, CCommon::LoadText(IDS_PRO_CLOSE_BUTTON));
        ShowDlgCtrl(IDC_SIGNIN_STATUS_STATIC, s.signed_in || s.has_entitlement);
        //Nút đăng nhập chỉ khi chưa đăng nhập và không bật Chế độ ngoại tuyến hoàn toàn
        ShowDlgCtrl(IDC_SIGNIN_BUTTON3, !s.signed_in && !theApp.m_general_data.offline_mode);
        for (UINT id : { IDC_SIGNIN_LAST_STATIC, IDC_SIGNIN_TERMS_CHECK, IDC_SIGNIN_TERMS_LINK, IDC_SIGNIN_PRIVACY_LINK,
                         IDC_SIGNIN_MORE_STATIC, IDC_SIGNIN_PASSKEY_BUTTON, IDC_SIGNIN_QR_BUTTON, IDC_SIGNIN_OR_STATIC,
                         IDC_SIGNIN_GUEST_NOTE_STATIC })
            ShowDlgCtrl(id, false);
    }

    LayoutControls();
    CenterWindow(GetParent());
    GetDlgItem(m_mode == Mode::SignIn ? IDC_SIGNIN_BUTTON1 : IDC_SIGNIN_GUEST_BUTTON)->SetFocus();
    return FALSE;   //đã đặt tiêu điểm
}

void CSignInDlg::ShowDlgCtrl(UINT id, bool show)
{
    CWnd* p = GetDlgItem(id);
    if (p != nullptr)
        p->ShowWindow(show ? SW_SHOW : SW_HIDE);
}

// Xếp dọc các điều khiển đang hiện theo thứ tự 13.2, khung rộng tối đa 400 px, đệm 32 px; chiều cao chữ nhiều dòng
// đo theo phông thật nên bản dịch dài không bị cắt.
void CSignInDlg::LayoutControls()
{
    const int margin = DPI(32);
    const int width = DPI(400) - 2 * margin;
    const int button_h = DPI(40);
    int y = margin;

    auto text_height = [&](UINT id, int w) {
        CWnd* p = GetDlgItem(id);
        CString text;
        p->GetWindowText(text);
        CClientDC dc(p);
        CFont* old = dc.SelectObject(p->GetFont());
        CRect rc(0, 0, w, 0);
        dc.DrawText(text.IsEmpty() ? CString(_T(" ")) : text, rc, DT_CALCRECT | DT_WORDBREAK | DT_NOPREFIX);
        dc.SelectObject(old);
        return rc.Height();
    };
    auto visible = [&](UINT id) { return (GetDlgItem(id)->GetStyle() & WS_VISIBLE) != 0; };
    auto place = [&](UINT id, int x, int w, int h) { GetDlgItem(id)->MoveWindow(x, y, w, h, FALSE); };
    auto stack_text = [&](UINT id, int gap) {
        if (!visible(id)) return;
        y += gap;
        const int h = text_height(id, width);
        place(id, margin, width, h);
        y += h;
    };

    //Nhãn "Lần trước bạn dùng cách này" đứng trên đúng nút đã dùng (13.2)
    UINT last_id = 0;
    if (m_mode == Mode::SignIn)
    {
        const std::wstring& m = theApp.m_last_sign_in_method;
        if (m == L"google") last_id = IDC_SIGNIN_BUTTON1;
        else if (m == L"github") last_id = IDC_SIGNIN_BUTTON2;
        else if (m == L"email") last_id = IDC_SIGNIN_BUTTON3;
        else if (m == L"passkey") last_id = IDC_SIGNIN_PASSKEY_BUTTON;
        else if (m == L"qr") last_id = IDC_SIGNIN_QR_BUTTON;
    }
    ShowDlgCtrl(IDC_SIGNIN_LAST_STATIC, last_id != 0);
    auto stack_button = [&](UINT id, int gap) {
        if (!visible(id)) return;
        y += gap;
        if (id == last_id)
        {
            const int h = text_height(IDC_SIGNIN_LAST_STATIC, width);
            place(IDC_SIGNIN_LAST_STATIC, margin, width, h);
            y += h + DPI(2);
        }
        place(id, margin, width, button_h);
        y += button_h;
    };

    const int logo = DPI(48);
    place(IDC_SIGNIN_LOGO_STATIC, margin, logo, logo);
    y += logo;
    stack_text(IDC_SIGNIN_TITLE_STATIC, DPI(12));
    stack_text(IDC_SIGNIN_VALUE_STATIC, DPI(6));
    stack_text(IDC_SIGNIN_STATUS_STATIC, DPI(10));
    y += DPI(14);
    stack_button(IDC_SIGNIN_BUTTON1, DPI(10));
    stack_button(IDC_SIGNIN_BUTTON2, DPI(10));
    stack_button(IDC_SIGNIN_BUTTON3, DPI(10));
    if (visible(IDC_SIGNIN_TERMS_CHECK))
    {
        y += DPI(16);
        const int box = GetSystemMetrics(SM_CXMENUCHECK) + DPI(6);
        const int h = text_height(IDC_SIGNIN_TERMS_CHECK, width - box);
        place(IDC_SIGNIN_TERMS_CHECK, margin, width, h);
        y += h + DPI(4);
        //Hai liên kết trên một dòng, thẳng hàng với chữ của ô đánh dấu
        const int link_h = text_height(IDC_SIGNIN_TERMS_LINK, width);
        CClientDC dc(this);
        CFont* old = dc.SelectObject(GetFont());
        CString t1, t2;
        GetDlgItemText(IDC_SIGNIN_TERMS_LINK, t1);
        GetDlgItemText(IDC_SIGNIN_PRIVACY_LINK, t2);
        const int w1 = dc.GetTextExtent(t1).cx + DPI(4);
        const int w2 = dc.GetTextExtent(t2).cx + DPI(4);
        dc.SelectObject(old);
        place(IDC_SIGNIN_TERMS_LINK, margin + box, w1, link_h);
        place(IDC_SIGNIN_PRIVACY_LINK, margin + box + w1 + DPI(16), w2, link_h);
        y += link_h;
    }
    if (visible(IDC_SIGNIN_MORE_STATIC))
    {
        stack_text(IDC_SIGNIN_MORE_STATIC, DPI(20));
        stack_button(IDC_SIGNIN_PASSKEY_BUTTON, DPI(8));
        stack_button(IDC_SIGNIN_QR_BUTTON, DPI(8));
        stack_text(IDC_SIGNIN_OR_STATIC, DPI(16));
    }
    stack_button(IDC_SIGNIN_GUEST_BUTTON, DPI(m_mode == Mode::SignIn ? 8 : 20));
    stack_text(IDC_SIGNIN_GUEST_NOTE_STATIC, DPI(6));
    y += margin;

    CRect rc(0, 0, width + 2 * margin, y);
    AdjustWindowRectEx(rc, GetStyle(), FALSE, GetExStyle());
    SetWindowPos(nullptr, 0, 0, rc.Width(), rc.Height(), SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    Invalidate();
}

HBRUSH CSignInDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CBaseDialog::OnCtlColor(pDC, pWnd, nCtlColor);
    switch (pWnd->GetDlgCtrlID())
    {
    case IDC_SIGNIN_VALUE_STATIC:
    case IDC_SIGNIN_MORE_STATIC:
    case IDC_SIGNIN_OR_STATIC:
    case IDC_SIGNIN_LAST_STATIC:
    case IDC_SIGNIN_GUEST_NOTE_STATIC:
        pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));      //text-muted (13.2)
        break;
    default:
        break;
    }
    return hbr;
}

void CSignInDlg::OnDestroy()
{
    if (m_logo != NULL)
    {
        DestroyIcon(m_logo);
        m_logo = NULL;
    }
    CBaseDialog::OnDestroy();
}

void CSignInDlg::OnOK()
{
    //Enter không đóng màn: mỗi lối vào là một nút riêng
}

static HRESULT CALLBACK ConsentCallback(HWND hwnd, UINT msg, WPARAM, LPARAM lParam, LONG_PTR)
{
    if (msg == TDN_HYPERLINK_CLICKED)
    {
        //Chỉ mở đúng hai trang đã khai trong ForkConfig.h
        const std::wstring url = reinterpret_cast<LPCWSTR>(lParam);
        if (url == FORK_TSUDEV_TERMS_URL || url == FORK_TSUDEV_PRIVACY_URL)
            ShellExecuteW(hwnd, L"open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
    return S_OK;
}

// 13.4: bấm lối vào khi chưa tích ô ⇒ bảng tóm tắt hai văn bản + "Đồng ý và tiếp tục" / "Huỷ", không chỉ báo lỗi
bool CSignInDlg::EnsureTermsAccepted()
{
    if (IsDlgButtonChecked(IDC_SIGNIN_TERMS_CHECK) == BST_CHECKED)
        return true;
    const int ID_AGREE = 101;
    const CString title = CCommon::LoadText(IDS_SIGNIN_CONSENT_TITLE);
    CString body = CCommon::LoadText(IDS_SIGNIN_CONSENT_BODY);
    body += _T("\n\n<a href=\"") FORK_TSUDEV_TERMS_URL _T("\">") + CCommon::LoadText(IDS_SIGNIN_TERMS_LINK) + _T("</a>    <a href=\"")
        FORK_TSUDEV_PRIVACY_URL _T("\">") + CCommon::LoadText(IDS_SIGNIN_PRIVACY_LINK) + _T("</a>");
    const CString agree = CCommon::LoadText(IDS_SIGNIN_CONSENT_AGREE);
    TASKDIALOG_BUTTON buttons[] = { { ID_AGREE, agree.GetString() } };
    TASKDIALOGCONFIG cfg{};
    cfg.cbSize = sizeof(cfg);
    cfg.hwndParent = GetSafeHwnd();
    cfg.hInstance = AfxGetResourceHandle();
    cfg.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION | TDF_ENABLE_HYPERLINKS | TDF_POSITION_RELATIVE_TO_WINDOW;
    cfg.dwCommonButtons = TDCBF_CANCEL_BUTTON;
    cfg.pszWindowTitle = title.GetString();
    cfg.pszMainInstruction = title.GetString();
    cfg.pszContent = body.GetString();
    cfg.pButtons = buttons;
    cfg.cButtons = ARRAYSIZE(buttons);
    cfg.nDefaultButton = ID_AGREE;
    cfg.pfCallback = ConsentCallback;
    int pressed = 0;
    if (FAILED(TaskDialogIndirect(&cfg, &pressed, nullptr, nullptr)) || pressed != ID_AGREE)
        return false;
    CheckDlgButton(IDC_SIGNIN_TERMS_CHECK, BST_CHECKED);
    return true;
}

void CSignInDlg::StartSignIn(const wchar_t* method, const char* idp_hint)
{
    if (!EnsureTermsAccepted())
        return;
    theApp.m_last_sign_in_method = method;
    theApp.SaveConfig();
    theApp.m_pro_account.StartSignIn(idp_hint);
    EndDialog(IDOK);
}

void CSignInDlg::OpenUrl(const wchar_t* url)
{
    ShellExecuteW(NULL, L"open", url, NULL, NULL, SW_SHOWNORMAL);
}

void CSignInDlg::OnBnClickedButton1()
{
    if (m_mode == Mode::SignIn)
        StartSignIn(L"google", "google");
    else
    {
        //Bản tải trực tiếp/winget/Scoop: trang Pro trên web (ADR-0002 mục 2.3, PRODUCT_POLICY 7.3)
        const std::string url = ProAuth::ProPageUrl();
        OpenUrl(std::wstring(url.begin(), url.end()).c_str());
    }
}

void CSignInDlg::OnBnClickedButton2()
{
    if (m_mode == Mode::SignIn)
        StartSignIn(L"github", "github");
    else
        OpenUrl(TSUDEV_DONATE_URL);     //Ủng hộ không cần tài khoản: chỉ mở trang, không nhúng thông tin chuyển khoản
}

void CSignInDlg::OnBnClickedButton3()
{
    if (m_mode == Mode::SignIn)
        StartSignIn(L"email", "");
    else
        EndDialog(IDRETRY);             //Người gọi mở màn đăng nhập
}

// Khoá truy cập và mã QR đều nằm trên trang đăng nhập tsudev.com (CROSS_DEVICE_LOGIN mục 2: RP không tự dựng QR)
void CSignInDlg::OnBnClickedPasskey()
{
    StartSignIn(L"passkey", "");
}

void CSignInDlg::OnBnClickedQr()
{
    StartSignIn(L"qr", "");
}

void CSignInDlg::OnBnClickedGuest()
{
    EndDialog(IDCANCEL);
}
