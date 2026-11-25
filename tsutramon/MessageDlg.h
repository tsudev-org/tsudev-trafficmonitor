#pragma once
#include "afxwin.h"
#include "BaseDialog.h"

// Hộp thoại CMessageDlg

class CMessageDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CMessageDlg)

public:
	CMessageDlg(CWnd* pParent = NULL);   // Hàm khởi tạo chuẩn
	virtual ~CMessageDlg();

	void SetWindowTitle(LPCTSTR str);	//Thiết lập tiêu đề cửa sổ
	void SetInfoText(LPCTSTR str);		//Thiết lập tiêu đề thông điệp
	void SetMessageText(LPCTSTR str);	//Thiết lập nội dung thông điệp
	//void ShowLinkStatic(bool show = true) { m_show_link_ctrl = show; }
	//void SetLinkInfo(LPCTSTR text, LPCTSTR url);
    void SetMessageIcon(HICON hIcon);

	//Biểu tượng chuẩn của hệ thống cho hộp thoại thông điệp
	enum StandardIcon
	{
		SI_INFORMATION,
		SI_WARNING,
		SI_ERROR
	};

	//Thiết lập biểu tượng chuẩn của hệ thống cho hộp thoại thông điệp
	void SetStandarnMessageIcon(StandardIcon standard_icon);

// Dữ liệu hộp thoại
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MESSAGE_DIALOG };
#endif

protected:
	CEdit m_message_edit;
	CSize m_min_size;		//Kích thước tối thiểu của cửa sổ
	CStatic m_info_static;

	CString m_title;
	CString m_info;
	CString m_message;

	//CString m_link_text;
	//CString m_link_url;

    HICON m_icon{};
    CPoint m_icon_pos{};        //Vị trí của biểu tượng

	//bool m_show_link_ctrl{ false };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Hỗ trợ DDX/DDV

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	//afx_msg void OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
};
