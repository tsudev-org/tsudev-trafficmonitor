#pragma once
#include "DataManager.h"

// Hộp thoại COptionsDlg

class COptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(COptionsDlg)

public:
	COptionsDlg(CWnd* pParent = nullptr);   // Hàm khởi tạo chuẩn
	virtual ~COptionsDlg();

// Dữ liệu hộp thoại
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPTIONS_DIALOG };
#endif

    SettingData m_data;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Hỗ trợ DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedShowSecondCheck();
};
