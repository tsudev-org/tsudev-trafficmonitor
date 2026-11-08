/*Điều khiển tĩnh dùng để hiển thị ảnh
Gọi hàm SetPicture() khi cần để thiết lập ảnh tĩnh.
Khi vẽ lại, điều khiển sẽ gửi thông điệp WM_CONTROL_REPAINT tới cửa sổ cha,
và truyền con trỏ CWnd của điều khiển hiện tại qua wParam, truyền con trỏ CDC qua lParam
*/
#pragma once
#define WM_CONTROL_REPAINT (WM_USER + 1003)		//Thông điệp vẽ lại điều khiển

class CPictureStatic : public CStatic
{
public:
	CPictureStatic();
	~CPictureStatic();

	void SetPicture(UINT pic_id);
	void SetPicture(HBITMAP hBitmap);

protected:
	CDC m_memDC;
	CBitmap m_bitmap;
	CRect m_rect;
	BITMAP m_bm;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
};

