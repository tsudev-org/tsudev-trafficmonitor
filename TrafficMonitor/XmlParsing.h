#pragma once
////////////////////////////////////////////////////////////////////////////////////////////////////
// XmlParsing.h - Phần phân tích XML thuần (không phụ thuộc MFC): skin.xml và thông tin phiên bản.
//
// Tách khỏi CSkinFile/CSimpleXML để kiểm thử tự động và fuzz được (tests/XmlParsingTests.cpp,
// tests/fuzz/SkinXmlFuzz.cpp; job "Kiểm thử C++" trong build.yml). CSkinFile và CSimpleXML gọi lại các
// hàm ở đây nên hành vi với skin hợp lệ không đổi; skin bất thường bị từ chối hoặc giá trị bị kẹp:
//   * skin.xml tối đa 1 MB, tối đa 4096 phần tử, sâu tối đa 16 cấp;
//   * số đọc từ thuộc tính được kẹp vào khoảng hợp lệ (không còn atoi tràn số);
//   * chuỗi (tác giả, văn bản hiển thị, ánh xạ plugin) giới hạn độ dài, số mục giới hạn.
// Chỉ dùng thư viện chuẩn và tinyxml2.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <climits>
#include <cstddef>
#include <cstring>
#include <map>
#include <string>
#include <vector>
#include "tinyxml2/tinyxml2.h"

namespace XmlParsing
{
    //Giới hạn phòng thủ
    const size_t MAX_SKIN_XML_BYTES = 1024 * 1024;
    const int MAX_SKIN_XML_ELEMENTS = 4096;
    const int MAX_SKIN_XML_DEPTH = 16;
    const size_t MAX_TEXT_COLORS = 256;
    const size_t MAX_MAP_ENTRIES = 256;
    const size_t MAX_TEXT_BYTES = 256;          //tác giả, văn bản hiển thị, tên nút, ID plugin
    const size_t MAX_FONT_NAME_BYTES = 128;

    //Khoảng giá trị hợp lệ (trước khi nhân DPI)
    const int MAX_SKIN_SIZE = 4096;             //chiều rộng/cao cửa sổ, ảnh xem trước, chiều rộng mục
    const int MAX_SKIN_POS = 4096;              //tọa độ x/y trong khoảng [-MAX_SKIN_POS, MAX_SKIN_POS]
    const int MAX_TEXT_HEIGHT = 512;
    const int MAX_FONT_SIZE = 72;
    const int MAX_FONT_STYLE = 15;              //4 bit: đậm, nghiêng, gạch chân, gạch ngang
    const int MAX_ALIGN = 3;                    //LEFT, RIGHT, CENTER, SIDE
    const long MAX_COLOR = 0xFFFFFF;

    //Đọc số nguyên như atoi (bỏ khoảng trắng đầu, dấu +/-, các chữ số đầu tiên; không có số thì 0)
    //nhưng không tràn số, rồi kẹp vào [min_value, max_value].
    inline long ParseLongClamped(const char* str, long min_value, long max_value)
    {
        long long value = 0;
        if (str != nullptr)
        {
            const char* p = str;
            while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n' || *p == '\v' || *p == '\f')
                p++;
            bool negative = false;
            if (*p == '+' || *p == '-')
            {
                negative = (*p == '-');
                p++;
            }
            while (*p >= '0' && *p <= '9')
            {
                if (value < 100000000000LL)     //đủ lớn để vượt mọi giới hạn, không tràn long long
                    value = value * 10 + (*p - '0');
                p++;
            }
            if (negative)
                value = -value;
        }
        if (value < min_value)
            return min_value;
        if (value > max_value)
            return max_value;
        return static_cast<long>(value);
    }

    inline int ParseIntClamped(const char* str, int min_value, int max_value)
    {
        return static_cast<int>(ParseLongClamped(str, min_value, max_value));
    }

    //Giống CTinyXml2Helper::StringToBool: rỗng hoặc "0" là false
    inline bool ParseBool(const char* str)
    {
        return str != nullptr && str[0] != '\0' && strcmp(str, "0") != 0;
    }

    //Cắt chuỗi UTF-8 còn tối đa max_bytes byte, không cắt giữa một ký tự nhiều byte
    inline std::string TruncateUtf8(const char* str, size_t max_bytes)
    {
        if (str == nullptr)
            return std::string();
        size_t len = strlen(str);
        if (len <= max_bytes)
            return std::string(str, len);
        size_t cut = max_bytes;
        while (cut > 0 && (static_cast<unsigned char>(str[cut]) & 0xC0) == 0x80)
            cut--;
        return std::string(str, cut);
    }

    //Tìm nội dung giữa <node> và </node> đầu tiên (dùng cho thông tin phiên bản, cách làm của CSimpleXML).
    //Thẻ đóng được tìm SAU thẻ mở (trước đây tìm từ đầu chuỗi: thẻ đóng đứng trước thẻ mở làm tràn độ dài).
    inline std::wstring GetSimpleNode(const wchar_t* node, const std::wstring& content)
    {
        if (node == nullptr || node[0] == L'\0')
            return std::wstring();
        std::wstring node_start = std::wstring(L"<") + node + L">";
        std::wstring node_end = std::wstring(L"</") + node + L">";
        size_t index_start = content.find(node_start);
        if (index_start == std::wstring::npos)
            return std::wstring();
        size_t content_start = index_start + node_start.size();
        size_t index_end = content.find(node_end, content_start);
        if (index_end == std::wstring::npos)
            return std::wstring();
        return content.substr(content_start, index_end - content_start);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // skin.xml
    ////////////////////////////////////////////////////////////////////////////////////////////////

    struct SkinLayoutItem
    {
        int x{}, y{}, width{}, align{};
        bool show{};
    };

    struct SkinLayout
    {
        int width{}, height{};
        std::map<std::string, SkinLayoutItem> items;    //key: tên nút (up, down, cpu... hoặc tên nút plugin)
    };

    struct SkinXmlData
    {
        //<skin>
        bool has_text_color{};                          //có nút <text_color> (kể cả rỗng)
        std::vector<unsigned long> text_colors;
        bool specify_each_item_color{};
        std::string skin_author;                        //UTF-8
        bool has_font{};
        std::string font_name;                          //UTF-8
        int font_size{};
        int font_style{};
        std::map<std::string, std::string> display_text;   //tên nút -> văn bản (UTF-8)
        //<layout>
        int text_height{};
        bool no_label{};
        SkinLayout layout_l;
        SkinLayout layout_s;
        //<preview>
        int preview_width{}, preview_height{};
        int preview_l_x{}, preview_l_y{}, preview_s_x{}, preview_s_y{};
        //<plugin_map>
        std::map<std::string, std::string> plugin_map;  //tên nút -> ID plugin (UTF-8)
    };

    namespace Detail
    {
        inline const char* Attr(const tinyxml2::XMLElement* ele, const char* name)
        {
            const char* value = (ele != nullptr) ? ele->Attribute(name) : nullptr;
            return value != nullptr ? value : "";
        }

        inline const char* Text(const tinyxml2::XMLElement* ele)
        {
            const char* value = (ele != nullptr) ? ele->GetText() : nullptr;
            return value != nullptr ? value : "";
        }

        inline const char* Name(const tinyxml2::XMLElement* ele)
        {
            const char* value = (ele != nullptr) ? ele->Name() : nullptr;
            return value != nullptr ? value : "";
        }

        //Đếm phần tử và độ sâu (không đệ quy); false nếu vượt giới hạn
        inline bool CheckStructure(const tinyxml2::XMLDocument& doc)
        {
            int count = 0;
            std::vector<std::pair<const tinyxml2::XMLElement*, int>> stack;
            for (const tinyxml2::XMLElement* e = doc.FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
                stack.push_back({ e, 1 });
            while (!stack.empty())
            {
                auto [ele, depth] = stack.back();
                stack.pop_back();
                if (++count > MAX_SKIN_XML_ELEMENTS || depth > MAX_SKIN_XML_DEPTH)
                    return false;
                for (const tinyxml2::XMLElement* c = ele->FirstChildElement(); c != nullptr; c = c->NextSiblingElement())
                    stack.push_back({ c, depth + 1 });
            }
            return true;
        }

        inline void PutLimited(std::map<std::string, std::string>& map, const char* key, const char* value)
        {
            if (strlen(key) > MAX_TEXT_BYTES)
                return;
            if (map.size() >= MAX_MAP_ENTRIES && map.find(key) == map.end())
                return;
            map[key] = TruncateUtf8(value, MAX_TEXT_BYTES);
        }

        inline SkinLayoutItem ParseLayoutItem(const tinyxml2::XMLElement* ele)
        {
            SkinLayoutItem item;
            item.x = ParseIntClamped(Attr(ele, "x"), -MAX_SKIN_POS, MAX_SKIN_POS);
            item.y = ParseIntClamped(Attr(ele, "y"), -MAX_SKIN_POS, MAX_SKIN_POS);
            item.width = ParseIntClamped(Attr(ele, "width"), 0, MAX_SKIN_SIZE);
            int align = ParseIntClamped(Attr(ele, "align"), INT_MIN, INT_MAX);
            item.align = (align >= 0 && align <= MAX_ALIGN) ? align : 0;     //giá trị lạ: căn trái
            item.show = ParseBool(Attr(ele, "show"));
            return item;
        }

        inline SkinLayout ParseLayout(const tinyxml2::XMLElement* ele)
        {
            SkinLayout layout;
            layout.width = ParseIntClamped(Attr(ele, "width"), 0, MAX_SKIN_SIZE);
            layout.height = ParseIntClamped(Attr(ele, "height"), 0, MAX_SKIN_SIZE);
            for (const tinyxml2::XMLElement* c = ele->FirstChildElement(); c != nullptr; c = c->NextSiblingElement())
            {
                const char* name = Name(c);
                if (strlen(name) > MAX_TEXT_BYTES)
                    continue;
                if (layout.items.size() >= MAX_MAP_ENTRIES && layout.items.find(name) == layout.items.end())
                    continue;
                layout.items[name] = ParseLayoutItem(c);
            }
            return layout;
        }
    }

    //Phân tích nội dung skin.xml (byte, UTF-8). Trả về false nếu quá lớn, sai cú pháp hoặc vượt giới hạn cấu trúc.
    //Phần tử gốc có tên bất kỳ (như bản cũ); các nút không biết bị bỏ qua.
    inline bool ParseSkinXml(const char* data, size_t size, SkinXmlData& out)
    {
        using namespace Detail;
        out = SkinXmlData();
        if (data == nullptr || size == 0 || size > MAX_SKIN_XML_BYTES)
            return false;
        tinyxml2::XMLDocument doc;
        if (doc.Parse(data, size) != tinyxml2::XML_SUCCESS)
            return false;
        if (!CheckStructure(doc))
            return false;
        const tinyxml2::XMLElement* root = doc.FirstChildElement();
        if (root == nullptr)
            return false;

        for (const tinyxml2::XMLElement* child = root->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
        {
            std::string name = Name(child);
            if (name == "skin")
            {
                for (const tinyxml2::XMLElement* item = child->FirstChildElement(); item != nullptr; item = item->NextSiblingElement())
                {
                    std::string item_name = Name(item);
                    if (item_name == "text_color")
                    {
                        //Danh sách màu ngăn cách bởi dấu phẩy (bỏ phần rỗng như CCommon::StringSplit)
                        out.has_text_color = true;
                        out.text_colors.clear();
                        std::string text = Text(item);
                        size_t start = 0;
                        while (start <= text.size() && out.text_colors.size() < MAX_TEXT_COLORS)
                        {
                            size_t comma = text.find(',', start);
                            std::string part = text.substr(start, comma == std::string::npos ? std::string::npos : comma - start);
                            size_t b = part.find_first_not_of(" \t\r\n");
                            if (b != std::string::npos)
                                out.text_colors.push_back(static_cast<unsigned long>(ParseLongClamped(part.c_str() + b, 0, MAX_COLOR)));
                            if (comma == std::string::npos)
                                break;
                            start = comma + 1;
                        }
                    }
                    else if (item_name == "specify_each_item_color")
                    {
                        out.specify_each_item_color = ParseBool(Text(item));
                    }
                    else if (item_name == "skin_author")
                    {
                        out.skin_author = TruncateUtf8(Text(item), MAX_TEXT_BYTES);
                    }
                    else if (item_name == "font")
                    {
                        out.has_font = true;
                        const char* font_name = Attr(item, "name");
                        out.font_name = (strlen(font_name) <= MAX_FONT_NAME_BYTES) ? font_name : "";   //tên quá dài: dùng phông mặc định
                        out.font_size = ParseIntClamped(Attr(item, "size"), 0, MAX_FONT_SIZE);
                        out.font_style = ParseIntClamped(Attr(item, "style"), 0, INT_MAX) & MAX_FONT_STYLE;
                    }
                    else if (item_name == "display_text")
                    {
                        for (const tinyxml2::XMLElement* t = item->FirstChildElement(); t != nullptr; t = t->NextSiblingElement())
                            PutLimited(out.display_text, Name(t), Text(t));
                    }
                }
            }
            else if (name == "layout")
            {
                out.text_height = ParseIntClamped(Attr(child, "text_height"), 0, MAX_TEXT_HEIGHT);
                out.no_label = ParseBool(Attr(child, "no_label"));
                for (const tinyxml2::XMLElement* l = child->FirstChildElement(); l != nullptr; l = l->NextSiblingElement())
                {
                    std::string layout_name = Name(l);
                    if (layout_name == "layout_l")
                        out.layout_l = ParseLayout(l);
                    else if (layout_name == "layout_s")
                        out.layout_s = ParseLayout(l);
                }
            }
            else if (name == "preview")
            {
                out.preview_width = ParseIntClamped(Attr(child, "width"), 0, MAX_SKIN_SIZE);
                out.preview_height = ParseIntClamped(Attr(child, "height"), 0, MAX_SKIN_SIZE);
                for (const tinyxml2::XMLElement* p = child->FirstChildElement(); p != nullptr; p = p->NextSiblingElement())
                {
                    std::string item_name = Name(p);
                    if (item_name == "l")
                    {
                        out.preview_l_x = ParseIntClamped(Attr(p, "x"), -MAX_SKIN_POS, MAX_SKIN_POS);
                        out.preview_l_y = ParseIntClamped(Attr(p, "y"), -MAX_SKIN_POS, MAX_SKIN_POS);
                    }
                    else if (item_name == "s")
                    {
                        out.preview_s_x = ParseIntClamped(Attr(p, "x"), -MAX_SKIN_POS, MAX_SKIN_POS);
                        out.preview_s_y = ParseIntClamped(Attr(p, "y"), -MAX_SKIN_POS, MAX_SKIN_POS);
                    }
                }
            }
            else if (name == "plugin_map")
            {
                for (const tinyxml2::XMLElement* p = child->FirstChildElement(); p != nullptr; p = p->NextSiblingElement())
                    PutLimited(out.plugin_map, Name(p), Text(p));
            }
        }
        return true;
    }
}
