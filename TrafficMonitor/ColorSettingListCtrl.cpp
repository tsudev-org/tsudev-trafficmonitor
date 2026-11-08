#include "stdafx.h"
#include "ColorSettingListCtrl.h"
#include "DrawCommonHelper.h"

IMPLEMENT_DYNAMIC(CColorSettingListCtrl, CListCtrl)

CColorSettingListCtrl::CColorSettingListCtrl()
{
}


CColorSettingListCtrl::~CColorSettingListCtrl()
{
}

void CColorSettingListCtrl::SetItemColor(int row, int col, COLORREF color)
{
    m_colors[row][col] = color;
}


COLORREF CColorSettingListCtrl::GetItemColor(int row, int col)
{
    return m_colors[row][col];
}

BEGIN_MESSAGE_MAP(CColorSettingListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CColorSettingListCtrl::OnNMCustomdraw)
END_MESSAGE_MAP()


void CColorSettingListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = CDRF_DODEFAULT;
	LPNMLVCUSTOMDRAW lplvdr = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	NMCUSTOMDRAW& nmcd = lplvdr->nmcd;
	switch (lplvdr->nmcd.dwDrawStage)	//Kiểm tra trạng thái   
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:			//Nếu cần đổi màu trước khi vẽ ITEM
		if (nmcd.dwItemSpec < static_cast<DWORD_PTR>(GetItemCount()))
		{
			//double range = m_item_rage_data[nmcd.dwItemSpec].data_value;
			//CDC* pDC = CDC::FromHandle(nmcd.hdc);		//Lấy DC vẽ
			//CRect item_rect, draw_rect;
			//GetSubItemRect(nmcd.dwItemSpec,m_draw_item_range_row, LVIR_BOUNDS, item_rect);	//Lấy vùng hình chữ nhật của ô cần vẽ
			//CDrawCommon::SetDrawRect(pDC, item_rect);		//Đặt vùng vẽ là cột hiện tại
			//draw_rect = item_rect;
			//if (draw_rect.Height() > 2 * m_margin)
			//{
			//	draw_rect.top += m_margin;
			//	draw_rect.bottom -= m_margin;
			//}
			//int width;
			//if (m_use_log_scale)	//Dùng tỉ lệ logarit (y=ln(x+1))
			//{
			//	range = std::log(range + 1);
			//	width = static_cast<int>(range*draw_rect.Width() / std::log(1000 + 1));
			//}
			//else		//Dùng tỉ lệ tuyến tính (y=x)
			//{
			//	width = static_cast<int>(range*draw_rect.Width() / 1000);
			//}
			//draw_rect.right = draw_rect.left + width;
			//pDC->FillSolidRect(draw_rect, m_item_rage_data[nmcd.dwItemSpec].color);

			////Sau khi vẽ xong cột hiện tại, đặt vùng vẽ thành vùng bên trái để tránh vùng của cột hiện tại bị ghi đè
			//CRect rect1{ item_rect };
			//rect1.left = 0;
			//rect1.right = item_rect.left;
			//CDrawCommon::SetDrawRect(pDC, rect1);

            CDC* pDC = CDC::FromHandle(nmcd.hdc);		//Lấy DC vẽ
            CRect item_rect;
            const int item_index = static_cast<int>(nmcd.dwItemSpec);
            auto& col_color_map = m_colors[item_index];
            for (auto iter = col_color_map.begin(); iter != col_color_map.end(); ++iter)
            {
                GetSubItemRect(item_index, iter->first, LVIR_BOUNDS, item_rect);	//Lấy vùng hình chữ nhật của ô cần vẽ
                //Thiết lập vùng cắt (clip) khi vẽ
                CDrawCommon::SetDrawRect(pDC, item_rect);
                //Vẽ bằng bộ đệm kép (double buffering)
                CDrawDoubleBuffer draw_double_buffer(pDC, item_rect);
                //Tô nền
                item_rect.MoveToXY(0, 0);
                draw_double_buffer.GetMemDC()->FillSolidRect(item_rect, GetSysColor(COLOR_WINDOW));
                //Vẽ hình chữ nhật màu
                item_rect.DeflateRect(m_margin, m_margin);
                draw_double_buffer.GetMemDC()->FillSolidRect(item_rect, iter->second);
                //Vẽ viền hình chữ nhật
                CDrawCommon drawer;
                drawer.Create(draw_double_buffer.GetMemDC(), this);
                drawer.DrawRectOutLine(item_rect, RGB(192, 192, 192));
            }
            //Sau khi vẽ xong cột hiện tại, đặt vùng vẽ thành vùng của cột đầu tiên để tránh vùng cột màu bị ghi đè
            CRect rect1{};
            GetSubItemRect(item_index, 1, LVIR_BOUNDS, rect1);	//Lấy vùng hình chữ nhật của ô cột thứ 1
            rect1.right = rect1.left;
            rect1.left = 0;
            CDrawCommon::SetDrawRect(pDC, rect1);

		}
		*pResult = CDRF_DODEFAULT;
		break;
	}
}
