#pragma once
#include "LinkStatic.h"
#include "BaseDialog.h"

// Hộp thoại CAboutDlg dùng cho mục menu “Giới thiệu” của ứng dụng

class CAboutDlg : public CBaseDialog
{
public:
    CAboutDlg();

    // Dữ liệu hộp thoại
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUTBOX };
#endif

protected:
    CLinkStatic m_mail;             //Siêu liên kết “Liên hệ tác giả”
    CLinkStatic m_acknowledgement;  //Siêu liên kết “Lời cảm ơn”
    CLinkStatic m_github;           //Siêu liên kết “GitHub”
    CLinkStatic m_gitee;            //Siêu liên kết “Dự án gốc”
    CLinkStatic m_tsudev;           //Siêu liên kết “tsudev.com”
    CLinkStatic m_license;          //Siêu liên kết “Giấy phép mã nguồn mở”
    CToolTipCtrl m_tool_tip;            //Chú giải (tooltip) khi trỏ chuột vào
    CLinkStatic m_translator_static;
    CLinkStatic m_openhardwaremonitor_link;
    CLinkStatic m_tinyxml2_link;

    CBitmap m_about_pic;

    virtual void DoDataExchange(CDataExchange* pDX);    // Hỗ trợ DDX/DDV
    CString GetDonateList();        //Tải danh sách người ủng hộ từ tệp tài nguyên
    virtual CString GetDialogName() const override;
    virtual bool InitializeControls() override;
    CRect CalculatePicRect();       //Tính vị trí của ảnh

    // Phần triển khai
protected:
    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    //  afx_msg void OnStnClickedStaticDonate();
protected:
    afx_msg LRESULT OnLinkClicked(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
