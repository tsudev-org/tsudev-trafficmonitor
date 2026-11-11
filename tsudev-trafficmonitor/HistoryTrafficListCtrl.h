#pragma once
#include "afxcmn.h"
#include "DrawCommon.h"
class CHistoryTrafficListCtrl :
	public CListCtrl
{
DECLARE_DYNAMIC(CHistoryTrafficListCtrl)
public:
	CHistoryTrafficListCtrl();
	~CHistoryTrafficListCtrl();

	void EnableDrawItemRange(bool draw = true) { m_draw_item_range = draw; }	//Thiết lập có vẽ hình chữ nhật biểu thị độ lớn giá trị trong ô của một cột nào đó hay không
	void SetDrawItemRangeRow(int row) { m_draw_item_range_row = row; }			//Thiết lập cột cần vẽ hình chữ nhật biểu thị độ lớn giá trị; lưu ý cột này phải là cột ngoài cùng bên phải, nếu không mọi cột ở bên phải nó sẽ không hiển thị được
	void SetDrawItemRangeData(int item, double range, COLORREF color);			//Thiết lập độ lớn dữ liệu (giá trị 1~1000) và màu của mục ở một hàng
	void SetDrawItemRangMargin(int margin) { m_margin = margin; }			//Thiết lập khoảng cách từ mép hình chữ nhật được vẽ đến viền ô; giá trị càng lớn thì hình chữ nhật càng mảnh, nhưng không được vượt quá một nửa khoảng cách dòng của danh sách
	void SetDrawItemRangInLogScale(bool log_scale);				//Thiết lập có dùng tỉ lệ logarit khi vẽ hình chữ nhật biểu thị độ lớn giá trị hay không, ngược lại dùng tỉ lệ tuyến tính

protected:
	struct ItemData
	{
		double data_value;		//Độ lớn giá trị mà hình chữ nhật cần vẽ biểu thị, phạm vi 0~1000
		COLORREF color;		//Màu của hình chữ nhật cần vẽ
	};
	bool m_draw_item_range{ false };		//Có cần vẽ hình chữ nhật biểu thị độ lớn giá trị trong ô của một cột nào đó hay không
	int m_draw_item_range_row{};		//Cột cần vẽ hình chữ nhật biểu thị độ lớn giá trị
	int m_margin{};
	vector<ItemData> m_item_rage_data;	//Dùng để biểu thị độ lớn dữ liệu cần vẽ và màu vẽ của từng mục
	bool m_use_log_scale{ false };		//Nếu là true thì vẽ hình chữ nhật theo tỉ lệ logarit, ngược lại dùng tỉ lệ tuyến tính

	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};

