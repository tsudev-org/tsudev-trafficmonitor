#pragma once
#define CALENDAR_WIDTH 7
#define CALENDAR_HEIGHT 6

struct DayTraffic
{
	int day;
	__int64 up_traffic;
	__int64 down_traffic;
	bool mixed;
	CRect rect;

	__int64 traffic() const
	{
		return up_traffic + down_traffic;
	}
};

class CCalendarHelper
{
public:
	CCalendarHelper();
	~CCalendarHelper();

	//Có phải năm nhuận không
	static bool IsLeapYear(int year);
	//Tính thứ trong tuần theo ngày (0~6: Chủ nhật~Thứ Bảy)
	static int CaculateWeekDay(int y, int m, int d);
	//Tính số ngày trong một tháng
	static int DaysInMonth(int year, int month);

	//Lấy dữ liệu lịch của tháng chỉ định và lưu vào mảng calendar
	//Nếu sunday_first là true thì lấy Chủ nhật làm ngày đầu tuần, ngược lại lấy Thứ Hai làm ngày đầu tuần
	static void GetCalendar(int year, int month, DayTraffic calendar[CALENDAR_HEIGHT][CALENDAR_WIDTH], bool sunday_first = true);

};

