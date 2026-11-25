#pragma once
#include "CommonData.h"
class CHistoryTrafficFile
{
public:
	CHistoryTrafficFile(const wstring& file_path);
	~CHistoryTrafficFile();

	void Save() const;
	void SaveTodayOnly() const;	// Chỉ cập nhật dòng thứ nhất (bộ đếm lines) và bản ghi hôm nay (dòng thứ hai), dùng để giảm I/O khi lưu thường xuyên
	void Load();
	void LoadSize();			//Chỉ đọc kích thước tệp
	void Merge(const CHistoryTrafficFile& history_traffic, bool ignore_same_data = false);		//Gộp với một đối tượng CHistoryTrafficFile khác. Nếu ignore_same_data là true thì bỏ qua các mục cùng ngày, ngược lại cộng dồn lưu lượng của các mục cùng ngày
	void OnDateChanged();		//Được gọi khi sang ngày mới: chuyển bản ghi hôm nay vào lịch sử và tạo bản ghi hôm nay mới

	const wstring& GetFilePath() const { return m_file_path; }
	const void SetFilePath(const wstring& file_path) { m_file_path = file_path; }
	
	// Lấy danh sách lưu lượng đầy đủ (dùng cho chức năng thống kê), trả về tham chiếu deque để tương thích với mã hiện có
	deque<HistoryTraffic>& GetTraffics() { if (m_cache_dirty) UpdateCache(); return m_traffics_cache; }
	const deque<HistoryTraffic>& GetTraffics() const { if (m_cache_dirty) UpdateCache(); return m_traffics_cache; }
	
	// Lấy bản ghi hôm nay (bản ghi đầu tiên)
	HistoryTraffic& GetTodayTraffic() { return m_today_traffic; }
	const HistoryTraffic& GetTodayTraffic() const { return m_today_traffic; }
	
	// Lấy danh sách liên kết lịch sử
	const list<HistoryTraffic>& GetHistoryTraffics() const { return m_history_traffics; }
	
	__int64 GetTodayUpTraffic() const { return m_today_up_traffic; }
	__int64 GetTodayDownTraffic() const { return m_today_down_traffic; }
	size_t Size() const { return m_size; }

private:
	void MormalizeData();		//Sắp xếp dữ liệu lưu lượng lịch sử và gộp các mục trùng
	bool IsTodayRecord() const;	//Kiểm tra ngày của bản ghi hôm nay có đúng không
	void UpdateCache() const;	//Cập nhật bộ nhớ đệm (dùng cho chức năng thống kê)
	void WriteTrafficRecord(ofstream& file, const HistoryTraffic& traffic) const;	//Ghi một bản ghi lưu lượng vào tệp
	//Ghi tệp an toàn: ghi vào tệp tạm, chỉ thay tệp thật khi ghi thành công (tránh mất lịch sử khi mất điện/đầy đĩa giữa lúc ghi)
	bool CommitTempFile(ofstream& file, const wstring& temp_path) const;
	wstring GetTempFilePath() const;
	HistoryTraffic CreateTodayTraffic() const;	//Tạo bản ghi hôm nay (ngày là ngày hiện tại, lưu lượng bằng 0)
	void InvalidateCache() const { m_cache_dirty = true; }	//Đánh dấu bộ nhớ đệm đã hết hạn

private:
	wstring m_file_path;
	HistoryTraffic m_today_traffic;        // Bản ghi hôm nay (lưu riêng, cập nhật thường xuyên)
	list<HistoryTraffic> m_history_traffics;	// Danh sách liên kết lịch sử (sắp xếp theo ngày từ lớn đến nhỏ)
	mutable deque<HistoryTraffic> m_traffics_cache;	// Bộ nhớ đệm: danh sách đầy đủ sau khi gộp (dùng cho chức năng thống kê, cập nhật khi cần)
	mutable bool m_cache_dirty{ true };	// Bộ nhớ đệm đã hết hạn hay chưa (cần cập nhật)
	__int64 m_today_up_traffic{};	//Lưu lượng tải lên đã dùng hôm nay
	__int64 m_today_down_traffic{};	//Lưu lượng tải xuống đã dùng hôm nay
	size_t m_size{};				//Số lượng bản ghi lưu lượng
};

