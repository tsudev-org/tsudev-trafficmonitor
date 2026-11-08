================================================================================
    THƯ VIỆN LỚP NỀN TẢNG MICROSOFT (MFC): Tổng quan dự án TrafficMonitor
===============================================================================

Trình hướng dẫn ứng dụng (Application Wizard) đã tạo ứng dụng TrafficMonitor này. Ứng dụng không chỉ minh họa cách dùng cơ bản các lớp MFC mà còn là điểm khởi đầu để viết ứng dụng của bạn.

Tệp này tóm tắt nội dung của từng tệp tạo nên ứng dụng TrafficMonitor.

TrafficMonitor.vcxproj
    Tệp dự án chính của VC++ được tạo bằng trình hướng dẫn, chứa thông tin phiên bản Visual C++ đã tạo tệp, cùng thông tin về nền tảng, cấu hình và tính năng dự án đã chọn.

TrafficMonitor.vcxproj.filters
    Tệp bộ lọc dự án VC++ được tạo bằng trình hướng dẫn. Tệp chứa thông tin liên kết giữa các tệp dự án và bộ lọc. IDE dùng liên kết này để hiển thị theo nhóm các tệp có phần mở rộng tương tự dưới một nút cụ thể. Ví dụ: tệp “.cpp” được liên kết với bộ lọc “Mã nguồn”.

TrafficMonitor.h
    Tệp header chính của ứng dụng.
    Tệp này include các header riêng khác của dự án (gồm Resource.h) và khai báo lớp ứng dụng CTrafficMonitorApp.

TrafficMonitor.cpp
    Tệp nguồn chính chứa lớp ứng dụng CTrafficMonitorApp.

TrafficMonitor.rc
    Danh sách tất cả tài nguyên Microsoft Windows mà chương trình sử dụng, gồm biểu tượng, bitmap và con trỏ lưu trong thư mục con RES. Có thể chỉnh sửa trực tiếp tệp này trong Microsoft Visual C++.

res\TrafficMonitor.ico
    Tệp biểu tượng dùng làm biểu tượng ứng dụng, được include trong tệp tài nguyên chính TrafficMonitor.rc.

res\TrafficMonitor.rc2
    Tệp chứa các tài nguyên không chỉnh sửa trong Microsoft Visual C++. Hãy đặt mọi tài nguyên mà trình chỉnh sửa tài nguyên không chỉnh được vào tệp này.

ForkConfig.h
    (Riêng bản Việt hóa) Khai báo tập trung repo, URL cập nhật và ngôn ngữ mặc định.


/////////////////////////////////////////////////////////////////////////////

Trình hướng dẫn tạo một lớp hộp thoại:

TrafficMonitorDlg.h, TrafficMonitorDlg.cpp - hộp thoại
    Các tệp này chứa lớp CTrafficMonitorDlg, định nghĩa hành vi hộp thoại chính của ứng dụng. Mẫu hộp thoại nằm trong TrafficMonitor.rc và có thể chỉnh sửa trong Microsoft Visual C++.

/////////////////////////////////////////////////////////////////////////////

Tính năng khác:

Điều khiển ActiveX
    Ứng dụng hỗ trợ sử dụng điều khiển ActiveX.

/////////////////////////////////////////////////////////////////////////////

Các tệp chuẩn khác:

StdAfx.h, StdAfx.cpp
    Dùng để tạo tệp header biên dịch trước (PCH) tên TrafficMonitor.pch và tệp kiểu đã biên dịch trước tên StdAfx.obj.

Resource.h
    Tệp header chuẩn dùng để định nghĩa ID tài nguyên mới. Microsoft Visual C++ sẽ đọc và cập nhật tệp này.

TrafficMonitor.manifest
	Tệp manifest ứng dụng mô tả các phụ thuộc của ứng dụng vào phiên bản cụ thể của side-by-side assembly. Trình nạp dùng thông tin này để nạp assembly phù hợp từ bộ đệm assembly. Manifest có thể được phân phối dưới dạng tệp .manifest bên ngoài đặt cùng thư mục với tệp thực thi, hoặc nhúng vào tệp thực thi dưới dạng tài nguyên.
/////////////////////////////////////////////////////////////////////////////

Ghi chú khác:

Trình hướng dẫn dùng “TODO:” để đánh dấu các phần mã nguồn cần bổ sung hoặc tùy biến.

Nếu ứng dụng dùng MFC dạng DLL dùng chung, bạn cần phân phối kèm các DLL của MFC. Nếu ngôn ngữ của ứng dụng khác với thiết lập vùng của hệ điều hành, cần phân phối kèm tài nguyên bản địa hóa tương ứng mfc110XXX.DLL.
Xem thêm phần phân phối lại ứng dụng Visual C++ trong tài liệu Microsoft Learn.

/////////////////////////////////////////////////////////////////////////////
