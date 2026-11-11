// CColorStatic
//Dẫn xuất từ CStatic, điều khiển Static dùng để hiển thị màu
#pragma once
#define WM_STATIC_CLICKED (WM_USER + 1001)

class CColorStatic : public CStatic
{
	DECLARE_DYNAMIC(CColorStatic)

public:
	CColorStatic();
	virtual ~CColorStatic();

	void SetFillColor(COLORREF fill_color);		//Thiết lập màu nền đơn sắc cần tô
	void SetColorNum(int color_num);			//Thiết lập số lượng màu
	void SetFillColor(int index, COLORREF fill_color);
	void SetLinkCursor(bool link_cursor = true);		//Thiết lập con trỏ chuyển thành hình siêu liên kết khi trỏ vào điều khiển
    void EnableWindow(bool enable);

protected:
	//COLORREF m_fill_color{ RGB(255, 255,255) };
	vector<COLORREF> m_colors;
	bool m_hover{ false };		//Là true khi chuột đang trỏ vào điều khiển
	bool m_link_cursor{ false };	//Có bật con trỏ hình siêu liên kết hay không

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	virtual void PreSubclassWindow();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
