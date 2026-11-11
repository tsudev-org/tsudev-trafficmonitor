#include "stdafx.h"
#include "SimpleXML.h"
#include "XmlParsing.h"


CSimpleXML::CSimpleXML(const wstring & xml_path)
{
	ifstream file_stream{ xml_path };
	if (file_stream.fail())
	{
		return;
	}
	//Đọc nội dung tệp
	string xml_str;
	char ch;
	while (file_stream.get(ch))
	{
		xml_str.push_back(ch);
	}
	if (!xml_str.empty() && xml_str.back() != L'\n')		//Đảm bảo cuối tệp có ký tự xuống dòng
		xml_str.push_back(L'\n');
	//Kiểm tra tệp có mã hóa utf8 hay không
	bool is_utf8;
	if (xml_str.size() >= 3 && xml_str[0] == -17 && xml_str[1] == -69 && xml_str[2] == -65)
	{
		//Nếu có BOM UTF8 thì xóa BOM
		is_utf8 = true;
		xml_str = xml_str.substr(3);
	}
	else
	{
		is_utf8 = false;
	}
	//Chuyển sang Unicode
	m_xml_content = CCommon::StrToUnicode(xml_str.c_str(), is_utf8);
}

CSimpleXML::CSimpleXML()
{
}


CSimpleXML::~CSimpleXML()
{
}

wstring CSimpleXML::GetNode(const wchar_t * node, const wchar_t * parent) const
{
	wstring node_content = _GetNode(parent, m_xml_content);
	return _GetNode(node, node_content);
}

wstring CSimpleXML::GetNode(const wchar_t * node) const
{
	return _GetNode(node, m_xml_content);
}

wstring CSimpleXML::_GetNode(const wchar_t * node, const wstring & content)
{
	//Logic thuần ở XmlParsing.h (có kiểm thử và fuzz)
	return XmlParsing::GetSimpleNode(node, content);
}
