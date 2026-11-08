#pragma once
#include "afxwin.h"
#include"StaticEx.h"
#include "PictureStatic.h"
#include "CSkinPreviewView.h"
#include "LinkStatic.h"
#include "BaseDialog.h"

// Hộp thoại CSkinDlg

class CSkinDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CSkinDlg)

public:
    CSkinDlg(CWnd* pParent = NULL);   // Hàm khởi tạo chuẩn
    virtual ~CSkinDlg();

    int m_skin_selected;			//Skin đã chọn

// Dữ liệu hộp thoại
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SKIN_DIALOG };
#endif

    const CSkinFile& GetSkinData() const { return m_skin_data; }

protected:
    virtual CString GetDialogName() const override;
    virtual bool InitializeControls() override;
    virtual void DoDataExchange(CDataExchange* pDX);    // Hỗ trợ DDX/DDV

protected:
    //Biến điều khiển
    CListBox m_skin_list_box;   //Danh sách chọn skin
    CLinkStatic m_skin_course;	//Siêu liên kết “Hướng dẫn tạo skin”
    CLinkStatic m_skin_download;	//Siêu liên kết “Tải thêm skin”
    CLinkStatic m_open_skin_dir_lnk;
    CSkinPreviewView* m_view;	//Lớp view của vùng xem trước
    CStatic m_preview_static;
    CStaticEx m_notify_static;	//Điều khiển static hiển thị thông tin gợi ý

    CSkinFile m_skin_data;		//Dữ liệu skin

    void ShowPreview();		//Hiển thị xem trước skin
    CRect CalculateViewRect();		//Tính kích thước view xem trước theo kích thước cửa sổ

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnLbnSelchangeList1();
    afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
    afx_msg LRESULT OnLinkClicked(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnBnClickedSkinAutoAdaptButton();
    virtual void OnOK();
    afx_msg void OnBnClickedSkinAutoAdaptCheck();
};
