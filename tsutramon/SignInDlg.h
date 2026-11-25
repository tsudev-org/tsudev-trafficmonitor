#pragma once
#include "BaseDialog.h"
#include "LinkStatic.h"
#include "ProAccount.h"

// Màn đăng nhập tài khoản tsudev và màn TsuDEV Pro/Ủng hộ theo khung chung AUTH_AND_ACCOUNT 13.2 (P9, QR1):
// logo 48 px, lời chào, một câu giá trị, các lối vào xếp dọc; ô đồng ý Điều khoản KHÔNG tích sẵn (13.4);
// "Thêm tuỳ chọn đăng nhập" (khoá truy cập, mã QR - cả hai do trang tsudev.com làm, CROSS_DEVICE_LOGIN mục 2);
// "Dùng ngay, không cần tài khoản" (chế độ khách, mục 14). Mọi lối vào mở trình duyệt hệ thống (mục 4):
// mật khẩu, khoá truy cập và mã QR chỉ có ở trang tsudev.com, không bao giờ trong Tsu Tramon.
// Bố cục tính lúc chạy (xếp dọc các điều khiển đang hiện), nên hai màn dùng chung một mẫu hộp thoại.

class CSignInDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CSignInDlg)

public:
    enum class Mode { SignIn, Pro };

    CSignInDlg(Mode mode, CWnd* pParent = NULL);
    virtual ~CSignInDlg();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SIGN_IN_DIALOG };
#endif

    // Dòng trạng thái TsuDEV Pro từ JWS đã kiểm chữ ký và còn hạn (PRODUCT_POLICY 7.4); dùng chung với thẻ Tài khoản
    static CString ProStatusText(const CProAccount::Status& s, int64_t now);

protected:
    Mode m_mode;
    CFont m_title_font;
    CFont m_small_font;
    HICON m_logo{};
    CLinkStatic m_terms_link;
    CLinkStatic m_privacy_link;

    void LayoutControls();
    void ShowDlgCtrl(UINT id, bool show);
    bool EnsureTermsAccepted();
    void StartSignIn(const wchar_t* method, const char* idp_hint);
    void OpenUrl(const wchar_t* url);

    virtual void DoDataExchange(CDataExchange* pDX);
    virtual CString GetDialogName() const override { return CString(); }

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnDestroy();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedButton2();
    afx_msg void OnBnClickedButton3();
    afx_msg void OnBnClickedPasskey();
    afx_msg void OnBnClickedQr();
    afx_msg void OnBnClickedGuest();
};
