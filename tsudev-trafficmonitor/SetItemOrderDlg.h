#pragma once
#include "BaseDialog.h"

// Hộp thoại CSetItemOrderDlg

class CSetItemOrderDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CSetItemOrderDlg)

public:
    CSetItemOrderDlg(CWnd* pParent = nullptr);   // Hàm khởi tạo chuẩn
    virtual ~CSetItemOrderDlg();

    //Thiết lập/lấy thứ tự hiển thị
    void SetItemOrder(const std::vector<int>& item_order);
    const std::vector<int>& GetItemOrder() const;

    //Thiết lập/lấy các mục hiển thị
    void SetDisplayItem(const DisplayItemSet& display_item);
    DisplayItemSet GetDisplayItem() const;

    void SetPluginDisplayItem(const StringSet& plugin_item);
    const StringSet& GetPluginDisplayItem() const;

    // Dữ liệu hộp thoại
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SELECT_ORDER_DIALOG };
#endif

private:
    CTaskbarItemOrderHelper m_item_order;   //Thứ tự các mục hiển thị
    CCheckListBox m_list_ctrl;
    DisplayItemSet m_display_item;        //Các mục tích hợp sẵn cần hiển thị
    StringSet m_plugin_item;            //Các mục plugin cần hiển thị
    std::vector<CommonDisplayItem> m_all_displayed_item;    //Tất cả mục hiển thị trong danh sách

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // Hỗ trợ DDX/DDV
    void ShowItem();
    void EnableCtrl(int list_selected);
    void EnableDlgCtrl(UINT id, bool enable);

    bool GetItemChecked(CommonDisplayItem item);
    void SaveItemChecked(CommonDisplayItem item, bool checked);

    // Kế thừa qua CBaseDialog
    virtual CString GetDialogName() const override;

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedMoveUpButton();
    afx_msg void OnBnClickedMoveDownButton();
    afx_msg void OnBnClickedRestoreDefaultButton();
    afx_msg void OnLbnSelchangeList1();
    afx_msg void OnCheckChanged();
    virtual void OnOK();
};
