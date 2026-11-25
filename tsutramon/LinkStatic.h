/*Kế thừa từ lớp CStatic, dùng làm siêu liên kết:
Gọi hàm SetURL để thiết lập siêu liên kết;
Gọi hàm SetLinkIsURL để thiết lập khi nhấp vào điều khiển sẽ mở siêu liên kết hay phản hồi lệnh
Nếu gọi SetLinkIsURL(false) thì sau khi nhấp vào điều khiển sẽ gửi thông điệp WM_LINK_CLICKED tới cửa sổ cha,
đồng thời truyền con trỏ điều khiển qua WPARAM.
Ngược lại, nhấp vào điều khiển sẽ mở siêu liên kết
*/
#pragma once

#define WM_LINK_CLICKED (WM_USER + 1002)

// CLinkStatic

class CLinkStatic : public CStatic
{
	DECLARE_DYNAMIC(CLinkStatic)

public:
	CLinkStatic();
	virtual ~CLinkStatic();
    void SetBackgroundColor(COLORREF background_color);

public:
	void SetURL(CString strURL);		//Thiết lập siêu liên kết
	CString GetURL() const;			//Lấy chuỗi siêu liên kết
	void SetLinkIsURL(bool enable) { m_link_is_url = enable; }
protected:
	bool m_link_is_url{ true };		//Nếu là true thì nhấp sẽ mở siêu liên kết, ngược lại gửi thông điệp nhấp tới cửa sổ cha
	bool m_bHot{ false };			//Là true khi chuột đang trỏ vào siêu liên kết
	CString m_strURL;				//Chuỗi siêu liên kết
    COLORREF m_back_color{ GetSysColor(COLOR_BTNFACE) };
    CFont m_hover_font;
    bool IsLinkValid() const;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
public:
	afx_msg void OnPaint();
	virtual void PreSubclassWindow();
};


