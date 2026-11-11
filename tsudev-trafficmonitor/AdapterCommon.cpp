#include "stdafx.h"
#include "AdapterCommon.h"


CAdapterCommon::CAdapterCommon()
{
}


CAdapterCommon::~CAdapterCommon()
{
}

void CAdapterCommon::GetAdapterInfo(vector<NetWorkConection>& adapters)
{
	adapters.clear();
	PIP_ADAPTER_INFO pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[sizeof(IP_ADAPTER_INFO)];		//Con trỏ cấu trúc PIP_ADAPTER_INFO lưu thông tin card mạng của máy
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);		//Lấy kích thước cấu trúc, dùng làm tham số cho GetAdaptersInfo
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);	//Gọi hàm GetAdaptersInfo để điền vào biến con trỏ pIpAdapterInfo; tham số stSize vừa là đầu vào vừa là đầu ra

	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		//Nếu hàm trả về ERROR_BUFFER_OVERFLOW
		//thì nghĩa là vùng nhớ truyền vào GetAdaptersInfo không đủ, đồng thời hàm trả ra stSize cho biết kích thước cần thiết
		//Đây cũng là lý do stSize vừa là đầu vào vừa là đầu ra
		delete[] (BYTE*)pIpAdapterInfo;	//Giải phóng vùng nhớ cũ
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];	//Cấp phát lại vùng nhớ để lưu toàn bộ thông tin card mạng
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);		//Gọi lại hàm GetAdaptersInfo để điền vào biến con trỏ pIpAdapterInfo
	}

	PIP_ADAPTER_INFO pIpAdapterInfoHead = pIpAdapterInfo;	//Lưu địa chỉ của phần tử đầu tiên trong danh sách liên kết pIpAdapterInfo
	if (ERROR_SUCCESS == nRel)
	{
		while (pIpAdapterInfo)
		{
			NetWorkConection connection;
			connection.description = pIpAdapterInfo->Description;
			connection.ip_address = CCommon::StrToUnicode(pIpAdapterInfo->IpAddressList.IpAddress.String);
			connection.subnet_mask = CCommon::StrToUnicode(pIpAdapterInfo->IpAddressList.IpMask.String);
			connection.default_gateway = CCommon::StrToUnicode(pIpAdapterInfo->GatewayList.IpAddress.String);

			adapters.push_back(connection);
			pIpAdapterInfo = pIpAdapterInfo->Next;
		}
	}
	//Giải phóng vùng nhớ
	if (pIpAdapterInfoHead)
	{
		delete[] (BYTE*)pIpAdapterInfoHead;
	}
	if (adapters.empty())
	{
		NetWorkConection connection{};
		connection.description = CCommon::UnicodeToStr(CCommon::LoadText(L"<", IDS_NO_CONNECTION, L">"));
		adapters.push_back(connection);
	}
}

void CAdapterCommon::RefreshIpAddress(vector<NetWorkConection>& adapters)
{
	vector<NetWorkConection> adapters_tmp;
	GetAdapterInfo(adapters_tmp);
	for (const auto& adapter_tmp : adapters_tmp)
	{
		for (auto& adapter : adapters)
		{
			if (adapter_tmp.description == adapter.description)
			{
				adapter.ip_address = adapter_tmp.ip_address;
				adapter.subnet_mask = adapter_tmp.subnet_mask;
				adapter.default_gateway = adapter_tmp.default_gateway;
			}
		}
	}
}

void CAdapterCommon::GetIfTableInfo(vector<NetWorkConection>& adapters, MIB_IFTABLE* pIfTable)
{
	if (pIfTable == nullptr)
		return;
	//Lần lượt tìm từng kết nối trong IfTable
	for (size_t i{}; i < adapters.size(); i++)
	{
		if (adapters[i].description.empty())
			continue;
		int index;
		index = FindConnectionInIfTable(adapters[i].description, pIfTable);
		if (index == -1)		//Nếu không tìm thấy bằng so khớp chính xác thì tìm lại bằng so khớp gần đúng
			index = FindConnectionInIfTableFuzzy(adapters[i].description, pIfTable);
		adapters[i].index = index;
		if (index < 0 || static_cast<DWORD>(index) >= pIfTable->dwNumEntries)	//IfTable rỗng: không truy cập ngoài phạm vi
			continue;
		const MIB_IFROW& row = pIfTable->table[index];
		GetInterfaceOctets(row, adapters[i].in_bytes, adapters[i].out_bytes);
		adapters[i].description_2 = GetIfRowDescription(row);
	}
}

void CAdapterCommon::GetAllIfTableInfo(vector<NetWorkConection>& adapters, MIB_IFTABLE * pIfTable)
{
	vector<NetWorkConection> adapters_tmp;
	GetAdapterInfo(adapters_tmp);		//Lấy địa chỉ IP
	adapters.clear();
	if (pIfTable == nullptr)
		return;
	for (size_t i{}; i < pIfTable->dwNumEntries; i++)
	{
		const MIB_IFROW& row = pIfTable->table[i];
		NetWorkConection connection;
		connection.description = connection.description_2 = GetIfRowDescription(row);
		connection.index = static_cast<int>(i);
		GetInterfaceOctets(row, connection.in_bytes, connection.out_bytes);
		for (size_t j{}; j < adapters_tmp.size(); j++)
		{
			if (!adapters_tmp[j].description.empty() && connection.description.find(adapters_tmp[j].description) != string::npos)
			{
				connection.ip_address = adapters_tmp[j].ip_address;
				connection.subnet_mask = adapters_tmp[j].subnet_mask;
				connection.default_gateway = adapters_tmp[j].default_gateway;
				break;
			}
		}
		adapters.push_back(connection);
	}
}

bool CAdapterCommon::GetInterfaceOctets(const MIB_IFROW& row, unsigned __int64& in_bytes, unsigned __int64& out_bytes)
{
	MIB_IF_ROW2 row2{};
	row2.InterfaceIndex = row.dwIndex;
	if (row.dwIndex != 0 && GetIfEntry2(&row2) == NO_ERROR)
	{
		in_bytes = row2.InOctets;
		out_bytes = row2.OutOctets;
		return true;
	}
	in_bytes = row.dwInOctets;
	out_bytes = row.dwOutOctets;
	return false;
}

unsigned __int64 CAdapterCommon::GetInterfaceSpeed(const MIB_IFROW& row)
{
	MIB_IF_ROW2 row2{};
	row2.InterfaceIndex = row.dwIndex;
	if (row.dwIndex != 0 && GetIfEntry2(&row2) == NO_ERROR)
		return (std::max)(row2.ReceiveLinkSpeed, row2.TransmitLinkSpeed);
	return row.dwSpeed;
}

string CAdapterCommon::GetIfRowDescription(const MIB_IFROW& row)
{
	const char* descr = reinterpret_cast<const char*>(row.bDescr);
	return string(descr, strnlen(descr, sizeof(row.bDescr)));
}

bool CAdapterCommon::HasGlobalIpv6Address()
{
	const ULONG flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_SKIP_FRIENDLY_NAME;
	ULONG size = 16 * 1024;
	std::vector<BYTE> buffer;
	ULONG rtn = ERROR_BUFFER_OVERFLOW;
	for (int i = 0; i < 3 && rtn == ERROR_BUFFER_OVERFLOW; i++)		//Số card mạng có thể thay đổi giữa hai lần gọi nên thử lại vài lần
	{
		buffer.resize(size);
		rtn = GetAdaptersAddresses(AF_INET6, flags, nullptr, reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data()), &size);
	}
	if (rtn != ERROR_SUCCESS)
		return false;
	for (auto p = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data()); p != nullptr; p = p->Next)
	{
		if (p->OperStatus != IfOperStatusUp || p->IfType == IF_TYPE_SOFTWARE_LOOPBACK)
			continue;
		for (auto addr = p->FirstUnicastAddress; addr != nullptr; addr = addr->Next)
		{
			if (addr->Address.lpSockaddr == nullptr || addr->Address.lpSockaddr->sa_family != AF_INET6)
				continue;
			if (addr->DadState != IpDadStatePreferred && addr->DadState != IpDadStateDeprecated)
				continue;
			if (addr->Address.iSockaddrLength < 24)
				continue;
			//Cấu trúc sockaddr_in6: family(2) + port(2) + flowinfo(4) + địa chỉ 16 byte. Đọc theo offset để không phụ thuộc winsock2/ws2ipdef.
			const BYTE* b = reinterpret_cast<const BYTE*>(addr->Address.lpSockaddr) + 8;
			bool link_local = (b[0] == 0xfe && (b[1] & 0xc0) == 0x80);
			bool unique_local = ((b[0] & 0xfe) == 0xfc);
			bool loopback_or_unspecified = std::all_of(b, b + 15, [](BYTE v) { return v == 0; }) && (b[15] == 0 || b[15] == 1);
			if (!link_local && !unique_local && !loopback_or_unspecified)
				return true;
		}
	}
	return false;
}

int CAdapterCommon::FindConnectionInIfTable(string connection, MIB_IFTABLE* pIfTable)
{
	for (size_t i{}; i < pIfTable->dwNumEntries; i++)
	{
		string descr = GetIfRowDescription(pIfTable->table[i]);
		if (descr == connection)
			return static_cast<int>(i);
	}
	return -1;
}

int CAdapterCommon::FindConnectionInIfTableFuzzy(string connection, MIB_IFTABLE* pIfTable)
{
	for (size_t i{}; i < pIfTable->dwNumEntries; i++)
	{
		string descr = GetIfRowDescription(pIfTable->table[i]);
		if (descr.empty() || connection.empty())	//Chuỗi rỗng "khớp" với mọi chuỗi, phải bỏ qua
			continue;
		size_t index;
		//Tìm chuỗi ngắn hơn trong chuỗi dài hơn
		if (descr.size() >= connection.size())
			index = descr.find(connection);
		else
			index = connection.find(descr);
		if (index != wstring::npos)
			return static_cast<int>(i);
	}
	if (pIfTable->dwNumEntries == 0)
		return -1;
	//Nếu vẫn không tìm thấy thì dùng thuật toán so khớp chuỗi để tìm
	double max_degree{};
	int best_index{};
	for (size_t i{}; i < pIfTable->dwNumEntries; i++)
	{
		string descr = GetIfRowDescription(pIfTable->table[i]);
		double degree = CCommon::StringSimilarDegree_LD(descr, connection);
		if (degree > max_degree)
		{
			max_degree = degree;
			best_index = static_cast<int>(i);
		}
	}
	return best_index;
}
