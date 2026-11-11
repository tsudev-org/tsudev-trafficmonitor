#pragma once
#include "DrawCommon.h"
#include "SkinFile.h"


// View CSkinPreviewView


class CSkinPreviewView : public CScrollView
{
	DECLARE_DYNCREATE(CSkinPreviewView)

protected:
	CSkinPreviewView();           // Hàm khởi tạo protected dùng cho tạo động
	virtual ~CSkinPreviewView();

public:
//#ifdef _DEBUG
//	virtual void AssertValid() const;
//#ifndef _WIN32_WCE
//	virtual void Dump(CDumpContext& dc) const;
//#endif
//#endif

//Hàm thành viên
public:
	void InitialUpdate();
	void SetSize(int width, int hight);
	void SetSkinData(CSkinFile* skin_data) { m_skin_data = skin_data; }

	//Biến thành viên
protected:
	CSize m_size;
	CPoint m_start_point;			//Vị trí bắt đầu vẽ

    CSkinFile* m_skin_data;

protected:
	virtual void OnDraw(CDC* pDC);      // Ghi đè để vẽ view này
	virtual void OnInitialUpdate();     // Lần đầu tiên sau khi khởi tạo

	DECLARE_MESSAGE_MAP()
};


