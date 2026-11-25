#pragma once
#include "tinyxml2/tinyxml2.h"
#include <string>
#include <functional>

class CTinyXml2Helper
{
public:

    //Đọc nội dung XML từ tệp
    static bool LoadXmlFile(tinyxml2::XMLDocument& doc, const wchar_t* file_path);

    //Duyệt một nút XML
    //fun: một đối tượng hàm, được gọi khi duyệt tới một nút
    static void IterateChildNode(tinyxml2::XMLElement* ele, std::function<void(tinyxml2::XMLElement*)> fun);

    //Lấy thuộc tính của một nút (giá trị trả về không bao giờ là con trỏ rỗng, không tìm thấy thì trả về chuỗi rỗng)
    static const char* ElementAttribute(tinyxml2::XMLElement* ele, const char* attr);

    //Lấy tên của một nút (giá trị trả về không bao giờ là con trỏ rỗng, không tìm thấy thì trả về chuỗi rỗng)
    static const char* ElementName(tinyxml2::XMLElement* ele);

    //Lấy văn bản của một nút (giá trị trả về không bao giờ là con trỏ rỗng, không tìm thấy thì trả về chuỗi rỗng)
    static const char* ElementText(tinyxml2::XMLElement* ele);

    static bool StringToBool(const char* str);
};
