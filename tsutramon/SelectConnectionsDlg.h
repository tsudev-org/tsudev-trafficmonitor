#pragma once
#include "CommonData.h"
#include "BaseDialog.h"

// Hộp thoại CSelectConnectionsDlg

class CSelectConnectionsDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CSelectConnectionsDlg)

public:
	CSelectConnectionsDlg(const StringSet& connections_hide, CWnd* pParent = nullptr);   // Hàm khởi tạo chuẩn
	virtual ~CSelectConnectionsDlg();

    const StringSet& GetData() const { return m_connections_hide; }

// Dữ liệu hộp thoại
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SELECT_CONNECTIONS_DIALOG };
#endif

private:
    CCheckListBox m_list_ctrl;
    std::vector<std::wstring> m_all_connection_names;
    StringSet m_connections_hide;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Hỗ trợ DDX/DDV
    virtual CString GetDialogName() const override;

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
};
