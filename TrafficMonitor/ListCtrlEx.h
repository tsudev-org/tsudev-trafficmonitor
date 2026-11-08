#pragma once
#include "afxcmn.h"
#include "DrawCommon.h"
#include <set>

#define IDC_ITEM_EDITBOX 1991
class CListCtrlEx :
	public CListCtrl
{
DECLARE_DYNAMIC(CListCtrlEx)
public:
	CListCtrlEx();
	~CListCtrlEx();

    void Edit(int row, int col);			//Chỉnh sửa ô chỉ định

    enum eEditColMethod       //Kiểu cột cần chỉnh sửa
    {
        EC_NONE,        //Không có
        EC_ALL,         //Tất cả
        EC_SPECIFIED    //Cột chỉ định
    };
    void SetEditColMethod(eEditColMethod method);
    void SetEditableCol(const std::initializer_list<int>& paras);   //Thiết lập các cột cho phép chỉnh sửa

private:
    CEdit m_item_edit;
    int m_edit_row{};
    int m_edit_col{};
    bool m_editing{};

    eEditColMethod m_edit_col_method{ EC_NONE };
    std::set<int> m_edit_cols;

protected:
    void EndEdit();

	DECLARE_MESSAGE_MAP()
    afx_msg void OnEnKillfocusEdit1();
    virtual void PreSubclassWindow();
    afx_msg BOOL OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnBeginScroll(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg LRESULT OnTabletQuerysystemgesturestatus(WPARAM wParam, LPARAM lParam);
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};

