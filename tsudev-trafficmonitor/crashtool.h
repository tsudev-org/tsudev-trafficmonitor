#pragma once

namespace CRASHREPORT
{
    /**@brief
        Bật chế độ dump
        Khi bật, nếu chương trình gặp sự cố sẽ tạo một tệp dump, tệp này nằm trong thư mục C:\Users\<tên người dùng>\AppData\Local\Temp
        Định dạng tên tệp: thời điểm sự cố_tên chương trình.exe.dmp
        như sau
        20150116174802_CrashShare.exe.dmp
    */
    void StartCrashReport();
}