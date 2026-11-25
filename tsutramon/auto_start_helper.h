#pragma once

#include "LegacyNames.h"

//Thư mục tác vụ tự khởi động trong Task Scheduler. Các thế hệ tên cũ dùng thư mục khác (LegacyNames.h);
//CTsuTramonApp::MigrateLegacyAutoRun() chuyển tác vụ cũ sang thư mục mới.
#define AUTO_START_TASK_FOLDER LegacyNames::kCurrent.task_folder

//path: nhận đường dẫn exe trong lệnh của tác vụ (nếu tác vụ tồn tại), kể cả khi hàm trả về false
bool is_auto_start_task_active_for_this_user(std::wstring* path, const wchar_t* folder = AUTO_START_TASK_FOLDER);
bool create_auto_start_task_for_this_user(bool runElevated);
bool delete_auto_start_task_for_this_user(const wchar_t* folder = AUTO_START_TASK_FOLDER);
