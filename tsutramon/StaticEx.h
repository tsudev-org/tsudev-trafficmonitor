/*
Lớp dẫn xuất của CStatic, dùng làm điều khiển văn bản có màu:
Gọi SetTextColor để thiết lập màu chữ;
Gọi SetWindowTextEx khi cần để thiết lập văn bản điều khiển
*/
#pragma once
#include "afxwin.h"
class CStaticEx :
	public CStatic
{
public:
	CStaticEx();
	~CStaticEx();

	//Kiểu căn chỉnh
	enum Alignment
	{
		LEFT,       //Căn trái
		RIGHT,      //Căn phải
		CENTER,     //Căn giữa
	};

	//Khi dùng điều khiển Static để vẽ văn bản có màu
public:
	void SetWindowTextEx(LPCTSTR lpszString, Alignment align = Alignment::LEFT);	//Thiết lập văn bản có màu cho điều khiển (cần dùng kèm SetTextColor)
	void SetTextColor(COLORREF textColor);		//Thiết lập màu chữ của điều khiển
	void SetBackColor(COLORREF back_color);		//Thiết lập màu nền của điều khiển
	CString GetString() const;			//Lấy văn bản của điều khiển

protected:
	bool m_color_text{ false };
	COLORREF m_text_color;	//Màu chữ của điều khiển
	COLORREF m_back_color;
	CString m_text;			//Văn bản trên điều khiển
	Alignment m_align{};		//Kiểu căn chỉnh văn bản
	bool m_draw_background_color{ false };	//Có cần tô màu nền cho điều khiển hay không

protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	virtual void PreSubclassWindow();
};

