#pragma once
#include "CVariant.h"
#include <map>
#include "CommonData.h"
class CIniHelper;

class CStrTable
{
public:
    CStrTable();
    ~CStrTable();

    void Init();

    // Tải chuỗi dịch
    const wstring& LoadText(const wstring& key) const;
    // Tải chuỗi dịch và thay các chuỗi dạng <%số thứ tự%> bằng tham số trong danh sách tham số biến đổi
    wstring LoadTextFormat(const wstring& key, const std::initializer_list<CVariant>& paras) const;

    const wstring& LoadMenuText(const wstring& key) const;

    const wstring& LoadText(const wstring& key, const wstring& section);

    // Lấy tên phông chữ mặc định
    const LanguageInfo& GetLanguageInfo() const { return m_language_info; }

    // Lấy tất cả ngôn ngữ được hỗ trợ
    const std::vector<LanguageInfo>& GetLanguageList() const { return m_language_list; }

    //Tìm gói ngôn ngữ ứng với cấu hình: khớp chính xác, nếu không thì cùng mã BCP-47 (ưu tiên cùng người dịch).
    //Trả về nguyên cấu hình nếu không tìm thấy hoặc cấu hình rỗng ("Theo hệ thống").
    LanguageInfo FindLanguage(const LanguageInfo& language) const;

    // Ngôn ngữ hiện tại có phải tiếng Trung giản thể không (dùng để chọn nguồn cập nhật mặc định và ngôn ngữ thông tin cập nhật)

protected:
    void ReadStringtableFronIni(const CIniHelper& ini);
    const std::map<std::wstring, std::wstring>& GetTextStringTable() const;
    const std::map<std::wstring, std::wstring>& GetMenuStringTable() const;

    static const wstring& LoadText(const wstring& key, const std::map<std::wstring, std::wstring>& string_table);
    static wstring LoadTextFormat(const wstring& key, const std::map<std::wstring, std::wstring>& string_table, const std::initializer_list<CVariant>& paras);


private:
    std::map<std::wstring, std::map<std::wstring, std::wstring>> m_string_table;    //Lưu các cặp khóa-giá trị của từng section trong tệp ini
    LanguageInfo m_language_info;                   //Ngôn ngữ hiện tại
    std::vector<LanguageInfo> m_language_list;      //Tất cả ngôn ngữ được hỗ trợ
};

