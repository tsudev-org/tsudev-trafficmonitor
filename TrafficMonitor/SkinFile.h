#pragma once
#include "CommonData.h"
#include "TinyXml2Helper.h"
#include "XmlParsing.h"
#include "DrawCommon.h"
#include <gdiplus.h>

class CSkinFile
{
public:
    CSkinFile();
    ~CSkinFile();


    //
    /**
     * @brief  Tải thông tin skin từ tệp
     * @param skin_name Tên skin
     */
    bool Load(const wstring& skin_name);

    //Thông tin skin
    struct SkinInfo
    {
        std::vector<COLORREF> text_color;   //Màu văn bản
        bool specify_each_item_color{};     //Có chỉ định màu cho từng mục hay không
        wstring skin_author;                //Tác giả của skin
        FontInfo font_info;                 //Thông tin phông chữ
        DispStrings display_text;    //Văn bản hiển thị của từng mục

        COLORREF TextColor(size_t i) const
        {
            if (i >= 0 && i < text_color.size())
                return text_color[i];
            else if (!text_color.empty())
                return text_color.front();
            else
                return 0;
        }
    };

    //Kiểu căn chỉnh
    enum class Alignment
    {
        LEFT,       //Căn trái
        RIGHT,      //Căn phải
        CENTER,     //Căn giữa
        SIDE        //Căn đều hai bên
    };

    //Thông tin bố cục của từng mục trong skin
    struct LayoutItem
    {
        int x{};            //Vị trí X
        int y{};            //Vị trí Y
        int width{};        //Chiều rộng
        Alignment align{};  //Kiểu căn chỉnh
        bool show{};        //Có hiển thị hay không
    };

    //Bố cục skin
    struct Layout
    {
        int width{};        //Chiều rộng
        int height{};       //Chiều cao
        std::map<CommonDisplayItem, LayoutItem> layout_items; //Thông tin bố cục của từng mục
        LayoutItem GetItem(CommonDisplayItem display_item) const
        {
            auto iter = layout_items.find(display_item);
            if (iter != layout_items.end())
                return iter->second;
            return LayoutItem();
        }
    };

    //Thông tin bố cục skin
    struct LayoutInfo
    {
        int text_height{};  //Chiều cao văn bản của skin
        bool no_label{};    //Có ẩn nhãn hay không
        Layout layout_l;    //Bố cục khi “Hiển thị thêm thông tin”
        Layout layout_s;    //Bố cục khi không “Hiển thị thêm thông tin”
    };

    //Thông tin ảnh xem trước của skin
    struct PreviewInfo
    {
        struct Pos
        {
            int x{};
            int y{};
        };
        int width{};    //Chiều rộng ảnh xem trước
        int height{};   //Chiều cao ảnh xem trước
        Pos l_pos;      //Vị trí cửa sổ trong ảnh xem trước khi “Hiển thị thêm thông tin”
        Pos s_pos;      //Vị trí cửa sổ trong ảnh xem trước khi không “Hiển thị thêm thông tin”
    };

    const SkinInfo& GetSkinInfo() const { return m_skin_info; }
    const LayoutInfo& GetLayoutInfo() const { return m_layout_info; }
    const PreviewInfo& GetPreviewInfo() const { return m_preview_info; }

    const CImage& GetBackgroundL() const { return m_background_l; }
    const CImage& GetBackgroundS() const { return m_background_s; }

    bool IsPNG() const { return m_is_png; }
    void SetAlpha(int alpha);       //Thiết lập độ mờ đục của cửa sổ chính, alpha: 0~255, chỉ có hiệu lực khi dùng nền png

    void SetSettingData(const SkinSettingData& setting_data);

    //Vẽ ảnh xem trước
    //pDC: CDC dùng để vẽ
    //rect: vùng vẽ
    void DrawPreview(CDC* pDC, CRect rect);

    //Vẽ giao diện chính
    void DrawInfo(CDC* pDC, bool show_more_info);

    static string GetDisplayItemXmlNodeName(DisplayItem display_item);

    //Lấy tất cả mục hiển thị trên skin này
    void GetSkinDisplayItems(std::set<CommonDisplayItem>& skin_all_items) const;

private:
    bool LoadFromXml(const wstring& file_path);     //Đọc dữ liệu skin từ tệp xml
    void LoadFromIni(const wstring& file_path);     //Đọc dữ liệu skin từ tệp ini (để tương thích với skin phiên bản cũ)

    CSkinFile::Layout LayoutFromParsed(const XmlParsing::SkinLayout& parsed);   //Bố cục từ dữ liệu đã phân tích (XmlParsing.h), đã nhân DPI

    struct DrawStr
    {
        CString label;
        CString value;
        CString GetStr() const
        {
            return label + value;
        }
    };

    static void DrawSkinText(IDrawCommon& drawer, DrawStr draw_str, CRect rect, COLORREF color, Alignment align);

    //Vẽ tất cả mục hiển thị trong giao diện chính ngoại trừ ảnh nền
    void DrawItemsInfo(IDrawCommon& drawer, Layout& layout, CFont& font) const;

private:
    SkinInfo m_skin_info;
    LayoutInfo m_layout_info;
    PreviewInfo m_preview_info;
    std::map<std::string, std::string> m_plugin_map;  //Quan hệ ánh xạ giữa tên plugin và tên nút xml. key là tên nút xml, value là ID plugin

    CFont m_font;
    CImage m_background_s;
    CImage m_background_l;
    bool m_is_png{};
    Gdiplus::Image* m_background_png_s{};
    Gdiplus::Image* m_background_png_l{};
    int m_alpha{ 255 };     //Độ mờ đục, chỉ có hiệu lực khi nền là png
    SkinSettingData m_setting_data;
    bool m_is_error{ false };
};
