#pragma once
#include "BaseDialog.h"

// Hộp thoại CAppAlreadyRuningDlg

class CAppAlreadyRuningDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CAppAlreadyRuningDlg)

public:
    CAppAlreadyRuningDlg(HWND handel, CWnd* pParent = nullptr);   // Hàm khởi tạo chuẩn
    virtual ~CAppAlreadyRuningDlg();

    // Dữ liệu hộp thoại
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_APP_ALREAD_RUNING_DIALOG };
#endif

private:
    HWND m_handle{};        //Handle cửa sổ chính của tiến trình tsudev-trafficmonitor đang chạy

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // Hỗ trợ DDX/DDV
    virtual CString GetDialogName() const override;

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedExitInstButton();
    afx_msg void OnBnClickedOpenSettingsButton();
    afx_msg void OnBnClickedShowHideMainWindowButton();
    afx_msg void OnBnClickedShowHideTaskbarWindowButton();
};
