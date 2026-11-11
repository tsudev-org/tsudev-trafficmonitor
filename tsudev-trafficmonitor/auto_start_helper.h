#pragma once

//Thư mục tác vụ tự khởi động trong Task Scheduler. Bản trước 26.11 (tên TrafficMonitor-VN) dùng thư mục cũ;
//CTsudevTrafficMonitorApp::MigrateLegacyAutoRun() chuyển tác vụ cũ sang thư mục mới.
#define AUTO_START_TASK_FOLDER L"\\tsudev-trafficmonitor"
#define LEGACY_AUTO_START_TASK_FOLDER L"\\TrafficMonitor"

//path: nhận đường dẫn exe trong lệnh của tác vụ (nếu tác vụ tồn tại), kể cả khi hàm trả về false
bool is_auto_start_task_active_for_this_user(std::wstring* path, const wchar_t* folder = AUTO_START_TASK_FOLDER);
bool create_auto_start_task_for_this_user(bool runElevated);
bool delete_auto_start_task_for_this_user(const wchar_t* folder = AUTO_START_TASK_FOLDER);
