// Kiểm thử tự động cho phần phân tích XML thuần (TrafficMonitor/XmlParsing.h): skin.xml và thông tin phiên bản.
// Build và chạy trong CI (build.yml, job "Kiểm thử C++"); chạy tay trên Windows:
//   cl /nologo /c /std:c++17 /EHsc /W3 /utf-8 TrafficMonitor\tinyxml2\tinyxml2.cpp
//   cl /nologo /std:c++17 /EHsc /W4 /WX /utf-8 /I TrafficMonitor tests\XmlParsingTests.cpp tinyxml2.obj /Fe:XmlParsingTests.exe
//   XmlParsingTests.exe TrafficMonitor\skins
// Đối số: thư mục skins; mọi skin.xml trong đó phải phân tích được và cho giá trị hợp lệ.

#include "XmlParsing.h"
#include <windows.h>
#include <fstream>
#include <iostream>
#include <iterator>

using namespace XmlParsing;

static int g_failed = 0;
static int g_passed = 0;

static void Check(bool cond, const char* expr, int line)
{
    if (cond)
    {
        g_passed++;
    }
    else
    {
        g_failed++;
        std::cerr << "THAT BAI (dong " << line << "): " << expr << "\n";
    }
}
#define CHECK(x) Check((x), #x, __LINE__)

static bool Parse(const std::string& xml, SkinXmlData& data)
{
    return ParseSkinXml(xml.data(), xml.size(), data);
}

static void TestNumbers()
{
    CHECK(ParseLongClamped("12", 0, 100) == 12);
    CHECK(ParseLongClamped("  -7", -10, 10) == -7);
    CHECK(ParseLongClamped("+5px", 0, 10) == 5);           // như atoi: dừng ở ký tự không phải số
    CHECK(ParseLongClamped("abc", -5, 5) == 0);
    CHECK(ParseLongClamped("", -5, 5) == 0);
    CHECK(ParseLongClamped(nullptr, 1, 5) == 1);            // 0 bị kẹp lên min
    CHECK(ParseLongClamped("999999999999999999999999", 0, 4096) == 4096);   // không tràn số
    CHECK(ParseLongClamped("-999999999999999999999999", -4096, 4096) == -4096);
    CHECK(ParseIntClamped("2147483648", INT_MIN, INT_MAX) == INT_MAX);
    CHECK(ParseIntClamped("-2147483649", INT_MIN, INT_MAX) == INT_MIN);
    CHECK(ParseIntClamped("300", 0, 72) == 72);
    CHECK(ParseBool("1"));
    CHECK(ParseBool("true"));
    CHECK(!ParseBool("0"));
    CHECK(!ParseBool(""));
    CHECK(!ParseBool(nullptr));

    CHECK(TruncateUtf8("abc", 5) == "abc");
    CHECK(TruncateUtf8("abcdef", 3) == "abc");
    CHECK(TruncateUtf8("a\xE1\xBA\xA1z", 2) == "a");           // không cắt giữa ký tự 3 byte ("ạ")
    CHECK(TruncateUtf8("a\xE1\xBA\xA1z", 4) == "a\xE1\xBA\xA1");
    CHECK(TruncateUtf8(nullptr, 4).empty());
}

static void TestSimpleNode()
{
    const std::wstring xml = L"<root><version>26.11.0905</version><GitHub><link>https://x/a.zip</link></GitHub></root>";
    CHECK(GetSimpleNode(L"version", xml) == L"26.11.0905");
    CHECK(GetSimpleNode(L"GitHub", xml) == L"<link>https://x/a.zip</link>");
    CHECK(GetSimpleNode(L"link", GetSimpleNode(L"GitHub", xml)) == L"https://x/a.zip");
    CHECK(GetSimpleNode(L"missing", xml).empty());
    CHECK(GetSimpleNode(L"", xml).empty());
    CHECK(GetSimpleNode(nullptr, xml).empty());
    CHECK(GetSimpleNode(L"a", L"<a>").empty());                       // thiếu thẻ đóng
    CHECK(GetSimpleNode(L"a", L"</a>x<a>").empty());                  // thẻ đóng đứng trước: trước đây trả về phần còn lại
    CHECK(GetSimpleNode(L"a", L"</a><a>ok</a>") == L"ok");
    CHECK(GetSimpleNode(L"a", L"<a></a>").empty());
}

static void TestSkinXml()
{
    SkinXmlData d;
    const std::string good =
        "\xEF\xBB\xBF<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        "<root>"
        "<skin><text_color>255, 65280 ,16711680,,</text_color><specify_each_item_color>1</specify_each_item_color>"
        "<skin_author>tsudev</skin_author><font name=\"Segoe UI\" size=\"9\" style=\"5\"/>"
        "<display_text><up>\xE2\x86\x91: </up><cpu>CPU: </cpu></display_text></skin>"
        "<layout text_height=\"20\" no_label=\"0\">"
        "<layout_l width=\"232\" height=\"80\"><up x=\"10\" y=\"4\" width=\"100\" align=\"3\" show=\"1\"/><cpu x=\"-5\" y=\"29\" width=\"100\" align=\"9\" show=\"0\"/><plg x=\"1\" y=\"2\" width=\"3\" align=\"1\" show=\"1\"/></layout_l>"
        "<layout_s width=\"232\" height=\"40\"><up x=\"10\" y=\"10\" width=\"100\" align=\"0\" show=\"1\"/></layout_s>"
        "</layout>"
        "<preview width=\"250\" height=\"128\"><l x=\"0\" y=\"0\"/><s x=\"0\" y=\"86\"/></preview>"
        "<plugin_map><plg>Plugin.Id</plg></plugin_map>"
        "</root>";
    CHECK(Parse(good, d));
    CHECK(d.has_text_color);
    CHECK(d.text_colors.size() == 3);
    CHECK(d.text_colors.size() == 3 && d.text_colors[0] == 255 && d.text_colors[1] == 65280 && d.text_colors[2] == 16711680);
    CHECK(d.specify_each_item_color);
    CHECK(d.skin_author == "tsudev");
    CHECK(d.has_font && d.font_name == "Segoe UI" && d.font_size == 9 && d.font_style == 5);
    CHECK(d.display_text.size() == 2 && d.display_text["up"] == "\xE2\x86\x91: " && d.display_text["cpu"] == "CPU: ");
    CHECK(d.text_height == 20 && !d.no_label);
    CHECK(d.layout_l.width == 232 && d.layout_l.height == 80 && d.layout_l.items.size() == 3);
    CHECK(d.layout_l.items["up"].x == 10 && d.layout_l.items["up"].align == 3 && d.layout_l.items["up"].show);
    CHECK(d.layout_l.items["cpu"].x == -5 && d.layout_l.items["cpu"].align == 0 && !d.layout_l.items["cpu"].show);   // align lạ -> căn trái
    CHECK(d.layout_s.height == 40 && d.layout_s.items.size() == 1);
    CHECK(d.preview_width == 250 && d.preview_height == 128 && d.preview_s_y == 86);
    CHECK(d.plugin_map.size() == 1 && d.plugin_map["plg"] == "Plugin.Id");

    // Nút text_color rỗng: vẫn đánh dấu có (CSkinFile mở rộng danh sách màu như trước)
    CHECK(Parse("<root><skin><text_color></text_color></skin></root>", d));
    CHECK(d.has_text_color && d.text_colors.empty());
    CHECK(Parse("<root><skin/></root>", d));
    CHECK(!d.has_text_color && !d.has_font);

    // Giá trị ngoài khoảng bị kẹp
    CHECK(Parse("<root><skin><text_color>-1,99999999999</text_color><font name=\"A\" size=\"500\" style=\"-3\"/></skin>"
                "<layout text_height=\"99999\"><layout_l width=\"-10\" height=\"100000\"><up x=\"-99999\" y=\"99999\" width=\"-1\" align=\"-2\"/></layout_l></layout>"
                "<preview width=\"99999\" height=\"-1\"><l x=\"99999\" y=\"-99999\"/></preview></root>", d));
    CHECK(d.text_colors.size() == 2 && d.text_colors[0] == 0 && d.text_colors[1] == 0xFFFFFF);
    CHECK(d.font_size == MAX_FONT_SIZE && d.font_style == 0);
    CHECK(d.text_height == MAX_TEXT_HEIGHT);
    CHECK(d.layout_l.width == 0 && d.layout_l.height == MAX_SKIN_SIZE);
    CHECK(d.layout_l.items["up"].x == -MAX_SKIN_POS && d.layout_l.items["up"].y == MAX_SKIN_POS);
    CHECK(d.layout_l.items["up"].width == 0 && d.layout_l.items["up"].align == 0);
    CHECK(d.preview_width == MAX_SKIN_SIZE && d.preview_height == 0 && d.preview_l_x == MAX_SKIN_POS && d.preview_l_y == -MAX_SKIN_POS);

    // Chuỗi dài bị cắt; tên phông quá dài bị bỏ (dùng phông mặc định)
    std::string long_text(1000, 'x');
    CHECK(Parse("<root><skin><skin_author>" + long_text + "</skin_author><font name=\"" + long_text + "\" size=\"9\"/></skin></root>", d));
    CHECK(d.skin_author.size() == MAX_TEXT_BYTES);
    CHECK(d.font_name.empty() && d.font_size == 9);

    // Số mục bị giới hạn
    std::string many = "<root><skin><display_text>";
    for (int i = 0; i < 1000; i++)
        many += "<n" + std::to_string(i) + ">t</n" + std::to_string(i) + ">";
    many += "</display_text></skin></root>";
    CHECK(Parse(many, d));
    CHECK(d.display_text.size() == MAX_MAP_ENTRIES);
    std::string colors = "<root><skin><text_color>";
    for (int i = 0; i < 1000; i++)
        colors += "1,";
    colors += "</text_color></skin></root>";
    CHECK(Parse(colors, d));
    CHECK(d.text_colors.size() == MAX_TEXT_COLORS);

    // Từ chối: rỗng, sai cú pháp, không có phần tử gốc, quá lớn, quá sâu, quá nhiều phần tử
    CHECK(!ParseSkinXml(nullptr, 0, d));
    CHECK(!Parse("", d));
    CHECK(!Parse("<root><skin></root>", d));
    CHECK(!Parse("<?xml version=\"1.0\"?>", d));
    CHECK(!Parse("<root>" + std::string(MAX_SKIN_XML_BYTES, ' ') + "</root>", d));
    std::string deep;
    for (int i = 0; i < MAX_SKIN_XML_DEPTH + 1; i++)
        deep += "<a>";
    for (int i = 0; i < MAX_SKIN_XML_DEPTH + 1; i++)
        deep += "</a>";
    CHECK(!Parse(deep, d));
    std::string ok_depth;
    for (int i = 0; i < MAX_SKIN_XML_DEPTH; i++)
        ok_depth += "<a>";
    for (int i = 0; i < MAX_SKIN_XML_DEPTH; i++)
        ok_depth += "</a>";
    CHECK(Parse(ok_depth, d));
    std::string wide = "<root>";
    for (int i = 0; i < MAX_SKIN_XML_ELEMENTS; i++)
        wide += "<x/>";
    wide += "</root>";
    CHECK(!Parse(wide, d));                                    // 4096 + gốc = 4097 phần tử
    // Thực thể ngoài/DTD không được mở rộng (tinyxml2 không hỗ trợ DTD): chỉ là văn bản
    CHECK(Parse("<!DOCTYPE r [<!ENTITY x SYSTEM \"file:///C:/Windows/win.ini\">]><root><skin><skin_author>&x;</skin_author></skin></root>", d)
        ? d.skin_author.find("[fonts]") == std::string::npos : true);
}

//Mọi skin.xml thật trong thư mục skins phải phân tích được, giá trị trong khoảng hợp lệ
static void TestRealSkins(const std::string& skins_dir)
{
    int count = 0;
    WIN32_FIND_DATAA data{};
    HANDLE find = FindFirstFileA((skins_dir + "\\*").c_str(), &data);
    CHECK(find != INVALID_HANDLE_VALUE);
    if (find == INVALID_HANDLE_VALUE)
        return;
    do
    {
        if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || data.cFileName[0] == '.')
            continue;
        std::ifstream f(skins_dir + "\\" + data.cFileName + "\\skin.xml", std::ios::binary);
        if (!f)
            continue;
        std::string xml((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
        SkinXmlData d;
        bool ok = Parse(xml, d);
        std::cout << "  skin " << data.cFileName << ": " << (ok ? "OK" : "LOI") << "\n";
        CHECK(ok);
        CHECK(d.has_text_color && !d.text_colors.empty());
        CHECK(d.has_font && !d.font_name.empty() && d.font_size > 0);
        CHECK(d.layout_l.width > 0 && d.layout_l.height > 0 && !d.layout_l.items.empty());
        CHECK(d.layout_s.width > 0 && d.layout_s.height > 0 && !d.layout_s.items.empty());
        CHECK(d.preview_width > 0 && d.preview_height > 0);
        count++;
    } while (FindNextFileA(find, &data));
    FindClose(find);
    CHECK(count >= 2);      //tsudev_DoThi_Sang, tsudev_DoThi_Toi
}

int main(int argc, char* argv[])
{
    std::string skins = (argc > 1) ? argv[1] : "TrafficMonitor\\skins";
    TestNumbers();
    TestSimpleNode();
    TestSkinXml();
    TestRealSkins(skins);
    std::cout << "XmlParsingTests: " << g_passed << " dat, " << g_failed << " that bai\n";
    return g_failed == 0 ? 0 : 1;
}
