#pragma once

// Include SDKDDKVer.h sẽ định nghĩa phiên bản nền tảng Windows cao nhất có thể dùng.

// Nếu muốn build ứng dụng cho nền tảng Windows cũ hơn, hãy include WinSDKVer.h và
// đặt macro _WIN32_WINNT thành nền tảng cần hỗ trợ, sau đó mới include SDKDDKVer.h.

#include <SDKDDKVer.h>
