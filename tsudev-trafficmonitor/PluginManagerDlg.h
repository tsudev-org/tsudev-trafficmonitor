#pragma once
#include "BaseDialog.h"
#include "LinkStatic.h"
#include "ListCtrlEx.h"

// Hộp thoại CPluginManagerDlg

class CPluginManagerDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CPluginManagerDlg)

public:
    CPluginManagerDlg(CWnd* pParent = nullptr);   // Hàm khởi tạo chuẩn
    virtual ~CPluginManagerDlg();

    // Dữ liệu hộp thoại
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_PLUGIN_MANAGER_DIALOG };
#endif

private:
    CListCtrlEx m_list_ctrl;
    int m_item_selected{ -1 };
    CMenu m_menu;
    CLinkStatic m_plugin_download_lnk;
    CLinkStatic m_plugin_dev_guide_lnk;
    CLinkStatic m_open_plugin_dir_lnk;
    CImageList m_plugin_icon_list;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // Hỗ trợ DDX/DDV
    void EnableControl();
    bool IsSelectedValid();
    bool IsSelectedPluginEnable();     //Plugin được chọn có khả dụng hay không

    enum Column
    {
        COL_FILE_NAME,
        COL_NAME,
        COL_VERSION,
        COL_STATUS,
        COL_MAX
    };

    DECLARE_MESSAGE_MAP()

    // Kế thừa qua CBaseDialog
    virtual CString GetDialogName() const override;
    virtual bool InitializeControls() override;

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMClickList1(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnBnClickedOptinsButton();
    afx_msg void OnBnClickedPluginInfoButton();
    afx_msg void OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnInitMenu(CMenu* pMenu);
    afx_msg void OnPluginDetail();
    afx_msg void OnPluginOptions();
    afx_msg void OnPluginDisable();
protected:
    afx_msg LRESULT OnLinkClicked(WPARAM wParam, LPARAM lParam);
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
