#pragma once
#include "BaseDialog.h"

// Hộp thoại CAutoAdaptSettingsDlg

class CAutoAdaptSettingsDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CAutoAdaptSettingsDlg)

public:
	CAutoAdaptSettingsDlg(TaskBarSettingData& data, CWnd* pParent = nullptr);   // Hàm khởi tạo chuẩn
	virtual ~CAutoAdaptSettingsDlg();

// Dữ liệu hộp thoại
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ATUO_ADAPT_SETTING_DIALOG };
#endif

private:
	CComboBox m_dark_mode_default_style_combo;
	CComboBox m_light_mode_default_style_combo;
	TaskBarSettingData& m_data;
    CToolTipCtrl m_toolTip;

private:
	void InitComboBox(CComboBox& combo_box, int style_sel);
	int GetComboBoxSel(const CComboBox& combo_box);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Hỗ trợ DDX/DDV
    virtual CString GetDialogName() const override;
    virtual bool InitializeControls() override;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};
