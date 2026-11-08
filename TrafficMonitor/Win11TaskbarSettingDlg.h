#pragma once
#include "afxdialogex.h"
#include "SpinEdit.h"
#include "BaseDialog.h"

// Hộp thoại CWin11TaskbarSettingDlg

class CWin11TaskbarSettingDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CWin11TaskbarSettingDlg)

public:
	CWin11TaskbarSettingDlg(TaskBarSettingData& data, CWnd* pParent = nullptr);   // Hàm khởi tạo chuẩn
	virtual ~CWin11TaskbarSettingDlg();

// Dữ liệu hộp thoại
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WIN11_TASKBAR_SETTING_DLG };
#endif

private:
    TaskBarSettingData& m_data;
    CSpinEdit m_window_offset_top_edit;
    CSpinEdit m_window_offset_left_edit;
    CSpinEdit m_widgets_width_edit;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // Hỗ trợ DDX/DDV
    virtual CString GetDialogName() const override;
    virtual bool InitializeControls() override;

    void EnableDlgCtrl(UINT id, bool enable);

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnBnClickedRestoreDefaultButton();
};
