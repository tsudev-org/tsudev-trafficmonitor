// CSkinPreviewView.cpp: tệp triển khai
//

#include "stdafx.h"
#include "TsudevTrafficMonitor.h"
#include "CSkinPreviewView.h"


// CSkinPreviewView

IMPLEMENT_DYNCREATE(CSkinPreviewView, CScrollView)

CSkinPreviewView::CSkinPreviewView()
{

}

CSkinPreviewView::~CSkinPreviewView()
{
}


BEGIN_MESSAGE_MAP(CSkinPreviewView, CScrollView)
END_MESSAGE_MAP()


// Vẽ CSkinPreviewView

void CSkinPreviewView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO:  Tính tổng kích thước của view này
	m_size.cx = 0;
	m_size.cy = 0;
	SetScrollSizes(MM_TEXT, m_size);


}

void CSkinPreviewView::OnDraw(CDC* pDC)
{
    CDrawCommon drawer;
    drawer.Create(pDC, nullptr);
    CRect view_rect;
    GetClientRect(view_rect);
    view_rect.right = (std::max)(view_rect.Width(), static_cast<int>(m_size.cx));
    view_rect.bottom = (std::max)(view_rect.Height(), static_cast<int>(m_size.cy));

    //Nếu skin có định dạng png thì vẽ nền bàn cờ 10x10
    if (m_skin_data->IsPNG())
    {
        int grid_size = theApp.DPI(10);

        // Kiểm tra kích thước canvas
        int rows = view_rect.Height() / grid_size + 1;  // Số hàng
        int cols = view_rect.Width() / grid_size + 1;  // Số cột

        // Duyệt từng ô lưới
        for (int row = 0; row < rows; ++row)
        {
            for (int col = 0; col < cols; ++col)
            {
                // Tính vùng hình chữ nhật của ô lưới hiện tại
                CRect rect(col * grid_size, row * grid_size,
                    (col + 1) * grid_size, (row + 1) * grid_size);

                // Xác định màu ô lưới hiện tại (tô xen kẽ)
                COLORREF color = ((row + col) % 2 == 0) ? RGB(204, 204, 204) : RGB(254, 254, 254);

                // Tô màu hình chữ nhật
                drawer.FillRect(rect, color);
            }
        }
    }
    //Vẽ nền màu đơn sắc
    else
    {
        drawer.FillRect(view_rect, GetSysColor(COLOR_WINDOW));
    }

    //Vẽ ảnh xem trước
    CRect draw_rect(CPoint(0, 0), m_size);
    m_skin_data->DrawPreview(pDC, draw_rect);
}


// Chẩn đoán CSkinPreviewView

//#ifdef _DEBUG
//void CSkinPreviewView::AssertValid() const
//{
//	CScrollView::AssertValid();
//}

//#ifndef _WIN32_WCE
//void CSkinPreviewView::Dump(CDumpContext& dc) const
//{
//	CScrollView::Dump(dc);
//}
//#endif
//#endif //_DEBUG

void CSkinPreviewView::InitialUpdate()
{
	OnInitialUpdate();
}
void CSkinPreviewView::SetSize(int width, int hight)
{
	m_size = CSize(width, hight);
	SetScrollSizes(MM_TEXT, m_size);
}

// Trình xử lý thông điệp của CSkinPreviewView
