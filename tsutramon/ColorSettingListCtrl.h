#pragma once
#include "afxcmn.h"
#include "DrawCommon.h"
class CColorSettingListCtrl :
	public CListCtrl
{
DECLARE_DYNAMIC(CColorSettingListCtrl)
public:
	CColorSettingListCtrl();
	~CColorSettingListCtrl();

    void SetItemColor(int row, int col, COLORREF color);
    COLORREF GetItemColor(int row, int col);
	void SetDrawItemRangMargin(int margin) { m_margin = margin; }			//Thiết lập khoảng cách từ mép hình chữ nhật được vẽ đến viền ô; giá trị càng lớn thì hình chữ nhật càng mảnh, nhưng không được vượt quá một nửa khoảng cách dòng của danh sách

protected:
	int m_margin{};
	std::map<int, std::map<int, COLORREF>> m_colors;	//Dùng để lưu màu của từng hàng, từng cột

	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};

