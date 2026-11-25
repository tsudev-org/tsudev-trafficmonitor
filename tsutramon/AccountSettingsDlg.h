#pragma once
#include "TabDlg.h"
#include <cstdint>

// Thẻ "Tài khoản tsudev" trong Tùy chọn (P9): đăng nhập/đăng xuất tài khoản tsudev, trạng thái TsuDEV Pro và
// công tắc Chế độ ngoại tuyến hoàn toàn (AUTH_AND_ACCOUNT 14.1-14.2). Luồng thật chạy ở CProAccount (theApp).

class CAccountSettingsDlg : public CTabDlg
{
    DECLARE_DYNAMIC(CAccountSettingsDlg)

public:
    CAccountSettingsDlg(CWnd* pParent = NULL);
    virtual ~CAccountSettingsDlg();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ACCOUNT_SETTINGS_DIALOG };
#endif

    bool m_offline_mode{ false };       //Giá trị công tắc, áp dụng khi bấm OK/Áp dụng (CTsuTramonDlg::ApplySettings)

protected:
    uint64_t m_shown_revision{ UINT64_MAX };

    void UpdateAccountUi(bool force = false);

    virtual void DoDataExchange(CDataExchange* pDX);

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnDestroy();
    afx_msg void OnBnClickedSignIn();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedRefresh();
    afx_msg void OnBnClickedPro();
    afx_msg void OnBnClickedSignOut();
    afx_msg void OnBnClickedOfflineMode();
};
