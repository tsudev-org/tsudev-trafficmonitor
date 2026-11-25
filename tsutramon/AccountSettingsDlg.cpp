// AccountSettingsDlg.cpp: thẻ "Tài khoản tsudev" trong Tùy chọn (P9 phần 3)
//

#include "stdafx.h"
#include "TsuTramon.h"
#include "AccountSettingsDlg.h"
#include "SignInDlg.h"

static const UINT_PTR ACCOUNT_UI_TIMER = 1;

IMPLEMENT_DYNAMIC(CAccountSettingsDlg, CTabDlg)

CAccountSettingsDlg::CAccountSettingsDlg(CWnd* pParent /*=NULL*/)
    : CTabDlg(IDD_ACCOUNT_SETTINGS_DIALOG, pParent)
{
}

CAccountSettingsDlg::~CAccountSettingsDlg()
{
}

void CAccountSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAccountSettingsDlg, CTabDlg)
    ON_WM_TIMER()
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_ACCOUNT_SIGN_IN_BUTTON, &CAccountSettingsDlg::OnBnClickedSignIn)
    ON_BN_CLICKED(IDC_ACCOUNT_CANCEL_BUTTON, &CAccountSettingsDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_ACCOUNT_REFRESH_BUTTON, &CAccountSettingsDlg::OnBnClickedRefresh)
    ON_BN_CLICKED(IDC_ACCOUNT_PRO_BUTTON, &CAccountSettingsDlg::OnBnClickedPro)
    ON_BN_CLICKED(IDC_ACCOUNT_SIGN_OUT_BUTTON, &CAccountSettingsDlg::OnBnClickedSignOut)
    ON_BN_CLICKED(IDC_OFFLINE_MODE_CHECK, &CAccountSettingsDlg::OnBnClickedOfflineMode)
END_MESSAGE_MAP()

BOOL CAccountSettingsDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    CheckDlgButton(IDC_OFFLINE_MODE_CHECK, m_offline_mode);
    UpdateAccountUi(true);
    //Luồng nền đổi trạng thái bất kỳ lúc nào (đăng nhập xong trong trình duyệt...): đọc lại định kỳ khi thẻ đang mở
    SetTimer(ACCOUNT_UI_TIMER, 300, nullptr);

    return TRUE;
}

static CString AccountErrorText(CProAccount::Error e)
{
    switch (e)
    {
    case CProAccount::Error::Offline: return CCommon::LoadText(IDS_ACCOUNT_ERR_OFFLINE);
    case CProAccount::Error::Listener: return CCommon::LoadText(IDS_ACCOUNT_ERR_LISTENER);
    case CProAccount::Error::Browser: return CCommon::LoadText(IDS_ACCOUNT_ERR_BROWSER);
    case CProAccount::Error::Cancelled: return CCommon::LoadText(IDS_ACCOUNT_ERR_CANCELLED);
    case CProAccount::Error::Denied: return CCommon::LoadText(IDS_ACCOUNT_ERR_DENIED);
    case CProAccount::Error::BadCallback: return CCommon::LoadText(IDS_ACCOUNT_ERR_BAD_CALLBACK);
    case CProAccount::Error::Network: return CCommon::LoadText(IDS_ACCOUNT_ERR_NETWORK);
    case CProAccount::Error::Server: return CCommon::LoadText(IDS_ACCOUNT_ERR_SERVER);
    case CProAccount::Error::InvalidToken: return CCommon::LoadText(IDS_ACCOUNT_ERR_INVALID_TOKEN);
    case CProAccount::Error::SessionExpired: return CCommon::LoadText(IDS_ACCOUNT_ERR_SESSION_EXPIRED);
    case CProAccount::Error::Storage: return CCommon::LoadText(IDS_ACCOUNT_ERR_STORAGE);
    default: return CString();
    }
}

void CAccountSettingsDlg::UpdateAccountUi(bool force)
{
    const CProAccount::Status s = theApp.m_pro_account.GetStatus();
    if (!force && s.revision == m_shown_revision)
        return;
    m_shown_revision = s.revision;
    const int64_t now = static_cast<int64_t>(_time64(nullptr));

    //Dòng trạng thái tài khoản (+ lỗi gần nhất)
    CString status;
    switch (s.op)
    {
    case CProAccount::Op::SigningIn: status = CCommon::LoadText(IDS_ACCOUNT_SIGNING_IN); break;
    case CProAccount::Op::Refreshing: status = CCommon::LoadText(IDS_ACCOUNT_REFRESHING); break;
    case CProAccount::Op::SigningOut: status = CCommon::LoadText(IDS_ACCOUNT_SIGNING_OUT); break;
    default:
    {
        const int64_t verified = s.last_online_ok > 0 ? s.last_online_ok : (s.has_entitlement ? s.entitlement.iat : 0);
        if (s.signed_in)
            status = verified > 0 ? CCommon::LoadTextFormat(IDS_ACCOUNT_SIGNED_IN_LAST, { theApp.FormatProDate(verified) })
                                  : CCommon::LoadText(IDS_ACCOUNT_SIGNED_IN);
        else
            status = CCommon::LoadText(IDS_ACCOUNT_SIGNED_OUT);
        break;
    }
    }
    const CString error = AccountErrorText(s.last_error);
    if (!error.IsEmpty() && s.op == CProAccount::Op::None)
        status += _T(" ") + error;
    SetDlgItemText(IDC_ACCOUNT_STATUS_STATIC, status);

    //Dòng TsuDEV Pro: chỉ dựa vào JWS đã kiểm chữ ký và còn hạn (PRODUCT_POLICY 7.4)
    SetDlgItemText(IDC_ACCOUNT_PRO_STATIC, CSignInDlg::ProStatusText(s, now));

    const bool busy = s.op != CProAccount::Op::None;
    const bool offline = m_offline_mode || theApp.m_general_data.offline_mode;
    EnableDlgCtrl(IDC_ACCOUNT_SIGN_IN_BUTTON, !s.signed_in && !busy && !offline);
    EnableDlgCtrl(IDC_ACCOUNT_CANCEL_BUTTON, s.op == CProAccount::Op::SigningIn);
    EnableDlgCtrl(IDC_ACCOUNT_REFRESH_BUTTON, s.signed_in && !busy && !offline);
    EnableDlgCtrl(IDC_ACCOUNT_SIGN_OUT_BUTTON, (s.signed_in || s.has_entitlement || s.op == CProAccount::Op::SigningIn) && s.op != CProAccount::Op::SigningOut);
}

void CAccountSettingsDlg::OnOK()
{
    m_offline_mode = (IsDlgButtonChecked(IDC_OFFLINE_MODE_CHECK) != 0);
    CTabDlg::OnOK();
}

void CAccountSettingsDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == ACCOUNT_UI_TIMER)
        UpdateAccountUi();
    CTabDlg::OnTimer(nIDEvent);
}

void CAccountSettingsDlg::OnDestroy()
{
    KillTimer(ACCOUNT_UI_TIMER);
    CTabDlg::OnDestroy();
}

void CAccountSettingsDlg::OnBnClickedSignIn()
{
    //Màn đăng nhập 13.2: chọn lối vào (Google, GitHub, email, khoá truy cập, mã QR) rồi mới mở trình duyệt
    CSignInDlg dlg(CSignInDlg::Mode::SignIn, this);
    dlg.DoModal();
    UpdateAccountUi(true);
}

void CAccountSettingsDlg::OnBnClickedCancel()
{
    theApp.m_pro_account.CancelSignIn();
}

void CAccountSettingsDlg::OnBnClickedRefresh()
{
    theApp.m_pro_account.RefreshNow();
}

void CAccountSettingsDlg::OnBnClickedPro()
{
    //Màn TsuDEV Pro/Ủng hộ cùng khung 13.2: mua/gia hạn trên web (ADR-0002 mục 2.3), ủng hộ không cần tài khoản
    CSignInDlg dlg(CSignInDlg::Mode::Pro, this);
    if (dlg.DoModal() == IDRETRY)
    {
        CSignInDlg sign_in(CSignInDlg::Mode::SignIn, this);
        sign_in.DoModal();
    }
    UpdateAccountUi(true);
}

void CAccountSettingsDlg::OnBnClickedSignOut()
{
    if (MessageBox(CCommon::LoadText(IDS_ACCOUNT_SIGN_OUT_CONFIRM), NULL, MB_OKCANCEL | MB_ICONQUESTION) != IDOK)
        return;
    theApp.SignOutTsudevAccount();
    UpdateAccountUi(true);
}

void CAccountSettingsDlg::OnBnClickedOfflineMode()
{
    m_offline_mode = (IsDlgButtonChecked(IDC_OFFLINE_MODE_CHECK) != 0);
    UpdateAccountUi(true);
}
