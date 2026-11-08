#pragma once
#include "ColorStatic.h"
#include "afxwin.h"
#include "ColorSettingListCtrl.h"
#include "BaseDialog.h"

// Hộp thoại CMainWndColorDlg

class CMainWndColorDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CMainWndColorDlg)

public:
	CMainWndColorDlg(const std::map<CommonDisplayItem, COLORREF>& colors, CWnd* pParent = NULL);   // Hàm khởi tạo chuẩn
	virtual ~CMainWndColorDlg();

    const std::map<CommonDisplayItem, COLORREF>& GetColors() const { return m_colors; }

// Dữ liệu hộp thoại
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN_COLOR_DIALOG };
#endif
protected:
    std::map<CommonDisplayItem, COLORREF> m_colors;
    CColorSettingListCtrl m_list_ctrl;

    virtual CString GetDialogName() const override;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Hỗ trợ DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
    afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
};
