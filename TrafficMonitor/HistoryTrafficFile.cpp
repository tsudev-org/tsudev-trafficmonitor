#include "stdafx.h"
#include "HistoryTrafficFile.h"
#include "Common.h"

CHistoryTrafficFile::CHistoryTrafficFile(const wstring& file_path)
	: m_file_path(file_path)
{
}

CHistoryTrafficFile::~CHistoryTrafficFile()
{
}

HistoryTraffic CHistoryTrafficFile::CreateTodayTraffic() const
{
	SYSTEMTIME current_time;
	GetLocalTime(&current_time);
	HistoryTraffic today_traffic;
	today_traffic.year = current_time.wYear;
	today_traffic.month = current_time.wMonth;
	today_traffic.day = current_time.wDay;
	today_traffic.up_kBytes = 0;
	today_traffic.down_kBytes = 0;
	today_traffic.mixed = false;
	return today_traffic;
}

void CHistoryTrafficFile::WriteTrafficRecord(ofstream& file, const HistoryTraffic& traffic) const
{
	char buff[64];
	if (traffic.mixed)
	{
		sprintf_s(buff, "%.4d/%.2d/%.2d %llu", traffic.year, traffic.month, 
			traffic.day, traffic.down_kBytes);
	}
	else
	{
		sprintf_s(buff, "%.4d/%.2d/%.2d %llu/%llu", traffic.year, traffic.month, 
			traffic.day, traffic.up_kBytes, traffic.down_kBytes);
	}
	file << buff << "\n";
}

void CHistoryTrafficFile::UpdateCache() const
{
	// Cập nhật bộ nhớ đệm: gộp bản ghi hôm nay và danh sách liên kết lịch sử
	m_traffics_cache.clear();
	m_traffics_cache.push_front(m_today_traffic);
	m_traffics_cache.insert(m_traffics_cache.end(), m_history_traffics.begin(), m_history_traffics.end());
	m_cache_dirty = false; // Đánh dấu bộ nhớ đệm đã được cập nhật
}

wstring CHistoryTrafficFile::GetTempFilePath() const
{
	return m_file_path + L".tmp";
}

bool CHistoryTrafficFile::CommitTempFile(ofstream& file, const wstring& temp_path) const
{
	file.flush();
	bool ok = file.good();
	file.close();
	ok = ok && !file.fail();
	if (ok)
		ok = (MoveFileExW(temp_path.c_str(), m_file_path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != FALSE);
	if (!ok)
		DeleteFileW(temp_path.c_str());		//Giữ nguyên tệp cũ
	return ok;
}

void CHistoryTrafficFile::Save() const
{
	const wstring temp_path = GetTempFilePath();
	ofstream file{ temp_path };
	if (!file.is_open())
	{
		return;
	}

	char buff[64];
	
	// Dòng thứ nhất: tổng số bản ghi (bản ghi hôm nay + bản ghi lịch sử)
	size_t total_size = 1 + m_history_traffics.size();
	sprintf_s(buff, "lines: \"%u\"", static_cast<unsigned int>(total_size));
	file << buff << "\n";

	// Dòng thứ hai: bản ghi hôm nay
	WriteTrafficRecord(file, m_today_traffic);

	// Từ dòng thứ ba trở đi: danh sách liên kết lịch sử
	for (const auto& history_traffic : m_history_traffics)
	{
		WriteTrafficRecord(file, history_traffic);
	}

	CommitTempFile(file, temp_path);
}

bool CHistoryTrafficFile::IsTodayRecord() const
{
	// Kiểm tra ngày của bản ghi hôm nay có đúng không
	SYSTEMTIME current_time;
	GetLocalTime(&current_time);
	
	return (m_today_traffic.year == current_time.wYear &&
			m_today_traffic.month == current_time.wMonth &&
			m_today_traffic.day == current_time.wDay);
}

void CHistoryTrafficFile::SaveTodayOnly() const
{
	// Lưu tăng dần: chỉ cập nhật dòng thứ nhất (bộ đếm lines) và dòng thứ hai (bản ghi hôm nay)
	// Tiền đề: ngày của bản ghi hôm nay là đúng (do chương trình tự quản lý, không cần đọc tệp để kiểm tra)
	
	// Nếu ngày của bản ghi hôm nay không đúng nghĩa là vừa sang ngày mới, dùng lưu đầy đủ
	if (!IsTodayRecord())
	{
		Save();
		return;
	}

	// Khi tệp chưa tồn tại thì dùng lưu đầy đủ (lưu lần đầu)
	if (!CCommon::FileExist(m_file_path.c_str()))
	{
		Save();
		return;
	}

	// Đọc các dòng còn lại của tệp (từ dòng 3 trở đi) để cập nhật tăng dần
	vector<string> remaining_lines;
	ifstream in_file{ m_file_path };
	if (in_file.is_open())
	{
		string line;
		int line_num = 0;
		while (std::getline(in_file, line))
		{
			line_num++;
			if (line_num > 2) // Bỏ qua hai dòng đầu
			{
				remaining_lines.push_back(line);
			}
		}
		in_file.close();
	}
	else
	{
		//Không đọc được tệp (ví dụ đang bị chương trình khác khóa): lưu đầy đủ từ dữ liệu trong bộ nhớ,
		//tránh ghi tệp mới chỉ có bản ghi hôm nay làm mất toàn bộ lịch sử
		Save();
		return;
	}

	// Ghi tệp đã cập nhật (qua tệp tạm)
	const wstring temp_path = GetTempFilePath();
	ofstream out_file{ temp_path };
	if (!out_file.is_open())
	{
		return;
	}

	// Dòng thứ nhất: bộ đếm lines (bản ghi hôm nay + bản ghi lịch sử)
	size_t total_size = 1 + m_history_traffics.size();
	char buff[64];
	sprintf_s(buff, "lines: \"%u\"", static_cast<unsigned int>(total_size));
	out_file << buff << "\n";

	// Dòng thứ hai: bản ghi hôm nay
	WriteTrafficRecord(out_file, m_today_traffic);

	// Các dòng còn lại: đọc từ tệp, ghi trực tiếp (không định dạng lại)
	for (const auto& line : remaining_lines)
	{
		out_file << line << "\n";
	}

	CommitTempFile(out_file, temp_path);
}

void CHistoryTrafficFile::Load()
{
	m_today_traffic = HistoryTraffic{}; // Khởi tạo bản ghi hôm nay
	m_history_traffics.clear(); // Xóa sạch danh sách liên kết lịch sử
	InvalidateCache(); // Đánh dấu bộ nhớ đệm đã hết hạn

	ifstream file{ m_file_path };
	string current_line, temp;
	HistoryTraffic traffic;
	bool is_first_data_line = true; // Đánh dấu có phải dòng dữ liệu đầu tiên hay không (bản ghi hôm nay)
	
	if (CCommon::FileExist(m_file_path.c_str()))
	{
		// Bỏ qua dòng đầu tiên (lines:)
		std::getline(file, current_line);
		
		while (!file.eof())
		{
			std::getline(file, current_line);

			if (current_line.size() < 12)
			{
				continue;
			}
			temp = current_line.substr(0, 4);
			traffic.year = atoi(temp.c_str());
			if (traffic.year < 1900 || traffic.year > 3000)
			{
				continue;
			}
			temp = current_line.substr(5, 2);
			traffic.month = atoi(temp.c_str());
			if (traffic.month < 1 || traffic.month > 12)
			{
				continue;
			}
			temp = current_line.substr(8, 2);
			traffic.day = atoi(temp.c_str());
			if (traffic.day < 1 || traffic.day > 31)
			{
				continue;
			}

			size_t separator_index = current_line.find(L'/', 11);
			traffic.mixed = (separator_index == wstring::npos);
			if (traffic.mixed)
			{
				temp = current_line.substr(11);
				traffic.down_kBytes = atoll(temp.c_str());
				traffic.up_kBytes = 0;
			}
			else
			{
				temp = current_line.substr(11, separator_index - 11);
				traffic.up_kBytes = atoll(temp.c_str());
				temp = current_line.substr(separator_index + 1);
				traffic.down_kBytes = atoll(temp.c_str());
			}
			
			if (traffic.year > 0 && traffic.month > 0 && traffic.day > 0 && traffic.kBytes() > 0)
			{
				if (is_first_data_line)
				{
					// Dòng dữ liệu đầu tiên là bản ghi hôm nay
					m_today_traffic = traffic;
					is_first_data_line = false;
				}
				else
				{
					// Phần còn lại là bản ghi lịch sử, chèn vào danh sách liên kết
					m_history_traffics.push_back(traffic);
				}
			}
		}
	}

	MormalizeData();
}

void CHistoryTrafficFile::LoadSize()
{
	ifstream file{ m_file_path };
	string current_line, temp;
	if (CCommon::FileExist(m_file_path.c_str()))
	{
		std::getline(file, current_line); // Đọc dòng đầu tiên
		size_t index = current_line.find("lines:");
		if (index != wstring::npos)
		{
			index = current_line.find("\"", index + 6);
			size_t index1 = current_line.find("\"", index + 1);
			temp = current_line.substr(index + 1, index1 - index - 1);
			const long long lines = atoll(temp.c_str());
			m_size = lines > 0 ? static_cast<size_t>(lines) : 0;
		}
	}
}

void CHistoryTrafficFile::Merge(const CHistoryTrafficFile& history_traffic, bool ignore_same_data)
{
	HistoryTraffic today_traffic = CreateTodayTraffic();

	// Gộp bản ghi hôm nay (chỉ gộp các bản ghi cùng ngày)
	// Lưu ý: nếu ignore_same_data=true nghĩa là đang khôi phục từ bản sao lưu, cần lấy giá trị lớn hơn thay vì cộng dồn để tránh cộng trùng
	if (HistoryTraffic::DateEqual(m_today_traffic, history_traffic.m_today_traffic))
	{
		if (ignore_same_data)
		{
			// Khi khôi phục từ bản sao lưu thì lấy giá trị lớn hơn (tránh cộng dồn trùng lặp)
			// Tệp sao lưu thường là dữ liệu đầy đủ khi chương trình thoát, tệp hiện tại có thể là dữ liệu chưa đầy đủ sau khi chương trình khởi động
			if (history_traffic.m_today_traffic.up_kBytes > m_today_traffic.up_kBytes)
			{
				m_today_traffic.up_kBytes = history_traffic.m_today_traffic.up_kBytes;
			}
			if (history_traffic.m_today_traffic.down_kBytes > m_today_traffic.down_kBytes)
			{
				m_today_traffic.down_kBytes = history_traffic.m_today_traffic.down_kBytes;
			}
		}
		else
		{
			// Khi gộp bình thường thì cộng dồn dữ liệu
			m_today_traffic.up_kBytes += history_traffic.m_today_traffic.up_kBytes;
			m_today_traffic.down_kBytes += history_traffic.m_today_traffic.down_kBytes;
		}
	}

	// Gộp danh sách liên kết lịch sử
	for (const HistoryTraffic& traffic : history_traffic.m_history_traffics)
	{
		// Bỏ qua bản ghi "tương lai" (thời gian hệ thống có thể đã bị chỉnh)
		if (HistoryTraffic::DateGreater(traffic, today_traffic))
		{
			continue; // Bỏ qua bản ghi "tương lai"
		}

		if (ignore_same_data)
		{
			// Nếu cần bỏ qua các mục cùng ngày thì dùng tìm kiếm tuyến tính (list không hỗ trợ truy cập ngẫu nhiên)
			auto it = std::find_if(m_history_traffics.begin(), m_history_traffics.end(),
				[&traffic](const HistoryTraffic& existing) {
					return HistoryTraffic::DateEqual(existing, traffic);
				});
			if (it != m_history_traffics.end())
			{
				continue; // Tìm thấy bản ghi cùng ngày, bỏ qua
			}
		}
		m_history_traffics.push_back(traffic);
	}
	
	MormalizeData();
	InvalidateCache(); // Đánh dấu bộ nhớ đệm đã hết hạn
}

void CHistoryTrafficFile::OnDateChanged()
{
	// Khi sang ngày mới, chuyển bản ghi hôm nay lên đầu danh sách liên kết lịch sử rồi tạo bản ghi hôm nay mới
	
	// Nếu bản ghi hôm nay có dữ liệu thì chuyển vào danh sách liên kết lịch sử
	if (m_today_traffic.kBytes() > 0)
	{
		m_history_traffics.push_front(m_today_traffic);
		// Sắp xếp ngay lập tức để đảm bảo dữ liệu nhất quán (theo ngày từ lớn đến nhỏ)
		if (m_history_traffics.size() >= 2)
		{
			m_history_traffics.sort(HistoryTraffic::DateGreater);
		}
	}
	
	// Tạo bản ghi hôm nay mới
	m_today_traffic = CreateTodayTraffic();
	
	// Cập nhật thống kê
	m_today_up_traffic = 0;
	m_today_down_traffic = 0;
	m_size = 1 + m_history_traffics.size();
	InvalidateCache(); // Đánh dấu bộ nhớ đệm đã hết hạn
}

void CHistoryTrafficFile::MormalizeData()
{
	HistoryTraffic today_traffic = CreateTodayTraffic();

	// Sắp xếp danh sách liên kết lịch sử trước (theo ngày từ lớn đến nhỏ) để tiện tìm kiếm và gộp về sau
	if (m_history_traffics.size() >= 2)
	{
		m_history_traffics.sort(HistoryTraffic::DateGreater);

		// Gộp các bản ghi cùng ngày
		auto it = m_history_traffics.begin();
		while (it != m_history_traffics.end())
		{
			auto next_it = it;
			++next_it;
			if (next_it != m_history_traffics.end() && HistoryTraffic::DateEqual(*it, *next_it))
			{
				it->up_kBytes += next_it->up_kBytes;
				it->down_kBytes += next_it->down_kBytes;
				m_history_traffics.erase(next_it);
			}
			else
			{
				++it;
			}
		}
	}

	// Dọn các bản ghi lịch sử có ngày muộn hơn ngày hiện tại (thời gian hệ thống có thể đã bị chỉnh)
	// Bản ghi lịch sử phải đều là ngày trong quá khứ, không được có ngày trong tương lai
	if (!m_history_traffics.empty())
	{
		auto it = m_history_traffics.begin();
		while (it != m_history_traffics.end())
		{
			// Nếu ngày của bản ghi lịch sử muộn hơn hôm nay nghĩa là bản ghi "tương lai", cần xóa
			if (HistoryTraffic::DateGreater(*it, today_traffic))
			{
				it = m_history_traffics.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	// Nếu ngày của m_today_traffic cũng muộn hơn ngày hiện tại nghĩa là thời gian hệ thống đã bị chỉnh, cần đặt lại
	if (HistoryTraffic::DateGreater(m_today_traffic, today_traffic))
	{
		// Nếu bản ghi hôm nay có dữ liệu thì nên chuyển vào lịch sử (nhưng nếu ngày muộn hơn hôm nay sẽ bị logic dọn dẹp phía trên xóa)
		// Đặt lại trực tiếp thành bản ghi hôm nay
		m_today_traffic = today_traffic;
	}

	// Tìm bản ghi hôm nay trong lịch sử (lịch sử có thể chứa dữ liệu của hôm nay)
	auto it = std::find_if(m_history_traffics.begin(), m_history_traffics.end(),
		[&today_traffic](const HistoryTraffic& traffic) {
			return HistoryTraffic::DateEqual(traffic, today_traffic);
		});

	if (it != m_history_traffics.end())
	{
		// Đã tìm thấy bản ghi hôm nay trong lịch sử
		if (HistoryTraffic::DateEqual(m_today_traffic, today_traffic))
		{
			// Nếu m_today_traffic cũng là của hôm nay thì gộp dữ liệu (tránh mất dữ liệu)
			m_today_traffic.up_kBytes += it->up_kBytes;
			m_today_traffic.down_kBytes += it->down_kBytes;
		}
		else
		{
			// Nếu m_today_traffic không phải của hôm nay thì thay bằng bản ghi trong lịch sử
			m_today_traffic = *it;
		}
		// Xóa bản ghi hôm nay khỏi lịch sử (vì bản ghi này chỉ nên nằm trong m_today_traffic)
		m_history_traffics.erase(it);
	}
	else if (!HistoryTraffic::DateEqual(m_today_traffic, today_traffic))
	{
		// Lịch sử không có bản ghi hôm nay, và m_today_traffic cũng không phải của hôm nay
		// Nếu m_today_traffic có dữ liệu thì nên chuyển vào danh sách liên kết lịch sử
		if (m_today_traffic.kBytes() > 0)
		{
			m_history_traffics.push_front(m_today_traffic);
			// Sắp xếp lại (vì đã chèn bản ghi mới)
			if (m_history_traffics.size() >= 2)
			{
				m_history_traffics.sort(HistoryTraffic::DateGreater);
			}
		}
		// Tạo bản ghi hôm nay mới
		m_today_traffic = today_traffic;
	}

	// Cập nhật thống kê lưu lượng hôm nay
	m_today_up_traffic = static_cast<__int64>(m_today_traffic.up_kBytes) * 1024;
	m_today_down_traffic = static_cast<__int64>(m_today_traffic.down_kBytes) * 1024;
	m_today_traffic.mixed = false;

	// Cập nhật tổng số bản ghi
	m_size = 1 + m_history_traffics.size();
	InvalidateCache(); // Đánh dấu bộ nhớ đệm đã hết hạn
}
