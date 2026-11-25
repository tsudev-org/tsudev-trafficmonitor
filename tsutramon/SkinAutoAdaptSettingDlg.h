#pragma once
#include "afxdialogex.h"
#include "BaseDialog.h"

// Hộp thoại CSkinAutoAdaptSettingDlg

class CSkinAutoAdaptSettingDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CSkinAutoAdaptSettingDlg)

public:
	CSkinAutoAdaptSettingDlg(CWnd* pParent = nullptr);   // Hàm khởi tạo chuẩn
	virtual ~CSkinAutoAdaptSettingDlg();

    int GetDarkModeSkin() const { return m_dark_mode_skin; }
    int GetLightModeSkin() const { return m_light_mode_skin; }

// Dữ liệu hộp thoại
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SKIN_AUTO_ADAPT_DLG };
#endif

private:
    CComboBox m_dark_mode_skin_combo;
    CComboBox m_light_mode_skin_combo;

    int m_dark_mode_skin{};
    int m_light_mode_skin{};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Hỗ trợ DDX/DDV
    virtual CString GetDialogName() const override;
    virtual bool InitializeControls() override;

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
};
