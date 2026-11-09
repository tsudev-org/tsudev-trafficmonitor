// Fuzzing (libFuzzer + AddressSanitizer của MSVC) cho phần phân tích XML thuần (TrafficMonitor/XmlParsing.h):
// skin.xml (qua tinyxml2) và tìm nút đơn giản của thông tin phiên bản. Ngoài lỗi bộ nhớ (ASan), harness kiểm các
// BẤT BIẾN: mọi giá trị trả về nằm trong khoảng hợp lệ, số mục và độ dài chuỗi không vượt giới hạn.
// Corpus hạt giống: các skin.xml thật trong TrafficMonitor/skins (CI chép vào thư mục corpus trước khi chạy).
// CI (build.yml, job "Kiểm thử C++"):
//   cl /c /std:c++17 /EHsc /utf-8 /Zi /fsanitize=address /fsanitize=fuzzer TrafficMonitor\tinyxml2\tinyxml2.cpp
//   cl /std:c++17 /EHsc /utf-8 /Zi /fsanitize=address /fsanitize=fuzzer /I TrafficMonitor tests\fuzz\SkinXmlFuzz.cpp tinyxml2.obj
//   SkinXmlFuzz.exe -max_total_time=120 skin-corpus

#include "XmlParsing.h"
#include <cstdint>
#include <cstdlib>

using namespace XmlParsing;

static void Require(bool cond)
{
    if (!cond)
        abort();
}

static bool InRange(int v, int lo, int hi)
{
    return v >= lo && v <= hi;
}

static void CheckLayout(const SkinLayout& layout)
{
    Require(InRange(layout.width, 0, SKIN_MAX_SIZE) && InRange(layout.height, 0, SKIN_MAX_SIZE));
    Require(layout.items.size() <= SKIN_MAX_MAP_ENTRIES);
    for (const auto& item : layout.items)
    {
        Require(item.first.size() <= SKIN_MAX_TEXT_BYTES);
        Require(InRange(item.second.x, -SKIN_MAX_POS, SKIN_MAX_POS) && InRange(item.second.y, -SKIN_MAX_POS, SKIN_MAX_POS));
        Require(InRange(item.second.width, 0, SKIN_MAX_SIZE) && InRange(item.second.align, 0, SKIN_MAX_ALIGN));
    }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // 1. skin.xml
    SkinXmlData d;
    if (ParseSkinXml(reinterpret_cast<const char*>(data), size, d))
    {
        Require(size > 0 && size <= MAX_SKIN_XML_BYTES);
        Require(d.text_colors.size() <= SKIN_MAX_TEXT_COLORS);
        for (auto c : d.text_colors)
            Require(c <= static_cast<unsigned long>(SKIN_MAX_COLOR));
        Require(d.has_text_color || d.text_colors.empty());
        Require(d.skin_author.size() <= SKIN_MAX_TEXT_BYTES);
        Require(d.font_name.size() <= SKIN_MAX_FONT_NAME_BYTES);
        Require(InRange(d.font_size, 0, SKIN_MAX_FONT_SIZE) && InRange(d.font_style, 0, SKIN_MAX_FONT_STYLE));
        Require(d.display_text.size() <= SKIN_MAX_MAP_ENTRIES && d.plugin_map.size() <= SKIN_MAX_MAP_ENTRIES);
        for (const auto& t : d.display_text)
            Require(t.first.size() <= SKIN_MAX_TEXT_BYTES && t.second.size() <= SKIN_MAX_TEXT_BYTES);
        for (const auto& t : d.plugin_map)
            Require(t.first.size() <= SKIN_MAX_TEXT_BYTES && t.second.size() <= SKIN_MAX_TEXT_BYTES);
        Require(InRange(d.text_height, 0, SKIN_MAX_TEXT_HEIGHT));
        CheckLayout(d.layout_l);
        CheckLayout(d.layout_s);
        Require(InRange(d.preview_width, 0, SKIN_MAX_SIZE) && InRange(d.preview_height, 0, SKIN_MAX_SIZE));
        Require(InRange(d.preview_l_x, -SKIN_MAX_POS, SKIN_MAX_POS) && InRange(d.preview_l_y, -SKIN_MAX_POS, SKIN_MAX_POS));
        Require(InRange(d.preview_s_x, -SKIN_MAX_POS, SKIN_MAX_POS) && InRange(d.preview_s_y, -SKIN_MAX_POS, SKIN_MAX_POS));
    }

    // 2. Số nguyên: luôn trong khoảng, không tràn
    std::string text(reinterpret_cast<const char*>(data), size);
    long v = ParseLongClamped(text.c_str(), -1000, 1000);
    Require(v >= -1000 && v <= 1000);

    // 3. Cắt UTF-8: không dài hơn giới hạn, là tiền tố của chuỗi gốc
    std::string cut = TruncateUtf8(text.c_str(), 16);
    Require(cut.size() <= 16 && text.compare(0, cut.size(), cut) == 0);

    // 4. Tìm nút đơn giản (thông tin phiên bản): kết quả là chuỗi con nằm giữa thẻ mở và thẻ đóng
    std::wstring w;
    for (size_t i = 0; i + 1 < size; i += 2)
        w.push_back(static_cast<wchar_t>(data[i] | (data[i + 1] << 8)));
    const wchar_t* names[] = { L"version", L"GitHub", L"link_x64", L"sha256_x64", L"contents_vi", L"a" };
    for (const wchar_t* name : names)
    {
        std::wstring node = GetSimpleNode(name, w);
        std::wstring narrow_node = GetSimpleNode(name, std::wstring(text.begin(), text.end()));
        Require(node.size() <= w.size() && narrow_node.size() <= text.size());
        if (!node.empty())
        {
            std::wstring open = std::wstring(L"<") + name + L">";
            size_t pos = w.find(open);
            Require(pos != std::wstring::npos);
            Require(w.compare(pos + open.size(), node.size(), node) == 0);
            Require(w.compare(pos + open.size() + node.size(), wcslen(name) + 3, std::wstring(L"</") + name + L">") == 0);
        }
    }
    return 0;
}
