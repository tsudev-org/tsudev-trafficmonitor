#pragma once
// Logic thuần đăng nhập tài khoản tsudev (OIDC + PKCE) và kiểm JWS quyền lợi TsuDEV Pro cho Tsu Tramon
// (P9; tsudev-standards AUTH_AND_ACCOUNT mục 4, 14.2-14.3; PRODUCT_POLICY mục 7.4; ADR-0002 mục 2.4-2.5).
// Chuyển từ bản C# đã chạy của Tsu Swico (src/Tsudev.Audit.Core/Pro) - cùng hợp đồng, cùng mã lỗi.
//
// KHÔNG phụ thuộc MFC. Phần thuần (JSON, base64url, URL, đối chiếu claim) build được trên mọi nền tảng để kiểm thử
// (tests/ProAuthTests.cpp, g++/clang + ASan/UBSan); phần mật mã (SHA-256, số ngẫu nhiên, ECDSA P-256) dùng BCrypt
// của Windows như UpdateSecurity.h và chỉ có khi _WIN32.
//
// VÌ SAO TỰ VIẾT mà không dùng thư viện JWT/JSON: hợp đồng chỉ có MỘT đường (ES256, typ cố định, khoá theo kid
// trong JWKS ghi cứng). Thư viện tổng quát mở thêm alg=none, HS256 với khoá công khai, jku/x5u... mà ta phải nhớ
// tắt từng cái. Bộ phân tích JSON ở đây cố ý hẹp: giới hạn kích thước/độ sâu/số phần tử, từ chối khoá trùng.
//
// Cách dùng (luồng đăng nhập, tầng mạng/giao diện nối sau):
//   auto req = ProAuth::NewAuthorizationRequest(port);               // state, nonce, PKCE (Windows)
//   mở trình duyệt hệ thống với req.Url();                            // KHÔNG WebView (AUTH mục 4)
//   auto cb = ProAuth::ParseCallbackRequestLine(dòng_đầu_HTTP);       // từ 127.0.0.1:<port>/callback
//   if (!req.StateMatches(cb.state) || !cb.IssuerOk()) => từ chối
//   đổi code lấy token (POST kTokenEndpoint, code_verifier = req.verifier) ; tải JWKS => ProAuth::ParseJwks()
//   ProAuth::ValidateIdToken(id_token, keys, kClientId, req.nonce, now, VerifyEs256)
//   GET kEntitlementsUrl (Bearer + X-Tsu-Installation) => {"jws": "..."} => ProAuth::VerifyEntitlement(...)

#include "ForkConfig.h"
#include <cstdint>
#include <cstring>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")
#endif

namespace ProAuth
{
    // ---------------------------------------------------------------------------------------------------------
    // Hợp đồng chung - chép NGUYÊN VĂN từ PRODUCT_POLICY 7.4, AUTH_AND_ACCOUNT 14.3 và discovery của tsudev.com;
    // URL khai ở ForkConfig.h (tools/check_urls.py canh).
    // ---------------------------------------------------------------------------------------------------------
    inline constexpr const char* kIssuer = FORK_TSUDEV_OIDC_ISSUER;
    inline constexpr const char* kAuthorizationEndpoint = FORK_TSUDEV_OIDC_AUTHORIZE_URL;
    inline constexpr const char* kTokenEndpoint = FORK_TSUDEV_OIDC_TOKEN_URL;
    inline constexpr const char* kRevocationEndpoint = FORK_TSUDEV_OIDC_REVOKE_URL;
    inline constexpr const char* kJwksUrl = FORK_TSUDEV_OIDC_JWKS_URL;
    inline constexpr const char* kEntitlementsUrl = FORK_TSUDEV_ENTITLEMENTS_URL;
    // Client OIDC native của Tsu Tramon (phiếu tsudev 20261125-DK3; /oidc/auth trả 303 từ 25/11/2026).
    // Cũng là aud của JWS quyền lợi và tham số app của trang Pro.
    inline constexpr const char* kClientId = "tsutramon";
    // offline_access để có refresh token; oidc-provider bỏ offline_access nếu thiếu prompt=consent.
    inline constexpr const char* kScope = "openid offline_access tsudev.entitlements";
    inline constexpr const char* kCallbackPath = "/callback";
    inline constexpr const char* kEntitlementType = "tsu-ent+jwt";
    inline constexpr const char* kAlgorithm = "ES256";        // thuật toán DUY NHẤT được nhận
    inline constexpr const char* kInstallationHeader = "X-Tsu-Installation";
    inline constexpr int64_t kMaxEntitlementLifetime = 7 * 24 * 60 * 60;   // exp - iat <= 7 ngày
    inline constexpr int64_t kClockSkew = 5 * 60;                           // lệch đồng hồ chấp nhận cho id_token
    inline constexpr size_t kMaxTokenSize = 16 * 1024;                      // JWS/id_token/JWKS dài hơn => từ chối

    inline std::string ProPageUrl() { return std::string(FORK_TSUDEV_PRO_URL_PREFIX) + kClientId; }

    // ---------------------------------------------------------------------------------------------------------
    // base64url (RFC 4648 mục 5), KHÔNG đệm. Giải mã chặt: ký tự ngoài bảng, '=' hay độ dài % 4 == 1 => lỗi.
    // ---------------------------------------------------------------------------------------------------------
    inline std::string Base64UrlEncode(const void* data, size_t size)
    {
        static const char* tbl = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
        const unsigned char* p = static_cast<const unsigned char*>(data);
        std::string out;
        out.reserve((size * 4 + 2) / 3);
        size_t i = 0;
        for (; i + 2 < size; i += 3)
        {
            uint32_t v = (uint32_t(p[i]) << 16) | (uint32_t(p[i + 1]) << 8) | p[i + 2];
            out += tbl[(v >> 18) & 63]; out += tbl[(v >> 12) & 63]; out += tbl[(v >> 6) & 63]; out += tbl[v & 63];
        }
        if (size - i == 1)
        {
            uint32_t v = uint32_t(p[i]) << 16;
            out += tbl[(v >> 18) & 63]; out += tbl[(v >> 12) & 63];
        }
        else if (size - i == 2)
        {
            uint32_t v = (uint32_t(p[i]) << 16) | (uint32_t(p[i + 1]) << 8);
            out += tbl[(v >> 18) & 63]; out += tbl[(v >> 12) & 63]; out += tbl[(v >> 6) & 63];
        }
        return out;
    }
    inline std::string Base64UrlEncode(const std::string& s) { return Base64UrlEncode(s.data(), s.size()); }

    inline bool Base64UrlDecode(const std::string& in, std::string& out)
    {
        out.clear();
        if (in.size() % 4 == 1)
            return false;
        auto val = [](char c) -> int {
            if (c >= 'A' && c <= 'Z') return c - 'A';
            if (c >= 'a' && c <= 'z') return c - 'a' + 26;
            if (c >= '0' && c <= '9') return c - '0' + 52;
            if (c == '-') return 62;
            if (c == '_') return 63;
            return -1;
        };
        out.reserve(in.size() * 3 / 4);
        uint32_t acc = 0;
        int bits = 0;
        for (char c : in)
        {
            int v = val(c);
            if (v < 0)
                return false;
            acc = (acc << 6) | uint32_t(v);
            bits += 6;
            if (bits >= 8)
            {
                bits -= 8;
                out += static_cast<char>((acc >> bits) & 0xFF);
            }
        }
        // Bit thừa ở cuối phải bằng 0 (dạng chuẩn tắc) - chặn nhiều chuỗi khác nhau cùng giải ra một giá trị
        if (bits > 0 && (acc & ((1u << bits) - 1)) != 0)
            return false;
        return true;
    }

    // So sánh thời gian hằng số (state, nonce đến từ cổng loopback mà tiến trình nào trên máy cũng gọi được)
    inline bool ConstantTimeEquals(const std::string& a, const std::string& b)
    {
        unsigned char diff = static_cast<unsigned char>(a.size() != b.size());
        size_t n = a.size() < b.size() ? a.size() : b.size();
        for (size_t i = 0; i < n; i++)
            diff |= static_cast<unsigned char>(a[i] ^ b[i]);
        return diff == 0;
    }

    // ---------------------------------------------------------------------------------------------------------
    // JSON tối giản, chặt (RFC 8259) cho JWS/JWKS/phản hồi token: số chỉ nhận nguyên trong int64 (claim thời gian),
    // số có phần thập phân/mũ được nhận nhưng đánh dấu không nguyên; khoá trùng trong một object => lỗi.
    // ---------------------------------------------------------------------------------------------------------
    struct JsonValue
    {
        enum class Type { Null, Bool, Number, String, Array, Object };
        Type type = Type::Null;
        bool b = false;
        bool is_int = false;
        int64_t i = 0;
        std::string s;
        std::vector<JsonValue> arr;
        std::map<std::string, JsonValue> obj;

        const JsonValue* Get(const std::string& key) const
        {
            if (type != Type::Object) return nullptr;
            auto it = obj.find(key);
            return it == obj.end() ? nullptr : &it->second;
        }
        bool GetString(const std::string& key, std::string& out) const
        {
            const JsonValue* v = Get(key);
            if (!v || v->type != Type::String) return false;
            out = v->s;
            return true;
        }
        bool GetInt(const std::string& key, int64_t& out) const
        {
            const JsonValue* v = Get(key);
            if (!v || v->type != Type::Number || !v->is_int) return false;
            out = v->i;
            return true;
        }
        bool Has(const std::string& key) const { return Get(key) != nullptr; }
    };

    class JsonParser
    {
    public:
        static constexpr int kMaxDepth = 8;
        static constexpr size_t kMaxNodes = 1024;

        static bool Parse(const std::string& text, JsonValue& out)
        {
            out = JsonValue();   // không để sót khoá của lần phân tích trước
            if (text.size() > kMaxTokenSize * 2)
                return false;
            JsonParser p(text);
            p.SkipWs();
            if (!p.ParseValue(out, 0))
                return false;
            p.SkipWs();
            return p.m_pos == text.size();
        }

    private:
        explicit JsonParser(const std::string& t) : m_t(t) {}
        const std::string& m_t;
        size_t m_pos = 0;
        size_t m_nodes = 0;

        void SkipWs()
        {
            while (m_pos < m_t.size() && (m_t[m_pos] == ' ' || m_t[m_pos] == '\t' || m_t[m_pos] == '\n' || m_t[m_pos] == '\r'))
                m_pos++;
        }
        bool Lit(const char* lit)
        {
            size_t n = std::strlen(lit);
            if (m_t.compare(m_pos, n, lit) != 0) return false;
            m_pos += n;
            return true;
        }
        bool ParseValue(JsonValue& v, int depth)
        {
            if (depth > kMaxDepth || ++m_nodes > kMaxNodes || m_pos >= m_t.size())
                return false;
            char c = m_t[m_pos];
            if (c == '{') return ParseObject(v, depth);
            if (c == '[') return ParseArray(v, depth);
            if (c == '"') { v.type = JsonValue::Type::String; return ParseString(v.s); }
            if (c == 't') { v.type = JsonValue::Type::Bool; v.b = true; return Lit("true"); }
            if (c == 'f') { v.type = JsonValue::Type::Bool; v.b = false; return Lit("false"); }
            if (c == 'n') { v.type = JsonValue::Type::Null; return Lit("null"); }
            if (c == '-' || (c >= '0' && c <= '9')) return ParseNumber(v);
            return false;
        }
        bool ParseObject(JsonValue& v, int depth)
        {
            v.type = JsonValue::Type::Object;
            m_pos++;   // {
            SkipWs();
            if (m_pos < m_t.size() && m_t[m_pos] == '}') { m_pos++; return true; }
            for (;;)
            {
                SkipWs();
                if (m_pos >= m_t.size() || m_t[m_pos] != '"') return false;
                std::string key;
                if (!ParseString(key)) return false;
                SkipWs();
                if (m_pos >= m_t.size() || m_t[m_pos] != ':') return false;
                m_pos++;
                SkipWs();
                JsonValue child;
                if (!ParseValue(child, depth + 1)) return false;
                if (!v.obj.emplace(std::move(key), std::move(child)).second)
                    return false;   // khoá trùng: hai bên đọc có thể hiểu khác nhau => từ chối
                SkipWs();
                if (m_pos >= m_t.size()) return false;
                if (m_t[m_pos] == ',') { m_pos++; continue; }
                if (m_t[m_pos] == '}') { m_pos++; return true; }
                return false;
            }
        }
        bool ParseArray(JsonValue& v, int depth)
        {
            v.type = JsonValue::Type::Array;
            m_pos++;   // [
            SkipWs();
            if (m_pos < m_t.size() && m_t[m_pos] == ']') { m_pos++; return true; }
            for (;;)
            {
                SkipWs();
                JsonValue child;
                if (!ParseValue(child, depth + 1)) return false;
                v.arr.push_back(std::move(child));
                SkipWs();
                if (m_pos >= m_t.size()) return false;
                if (m_t[m_pos] == ',') { m_pos++; continue; }
                if (m_t[m_pos] == ']') { m_pos++; return true; }
                return false;
            }
        }
        static int Hex(char c)
        {
            if (c >= '0' && c <= '9') return c - '0';
            if (c >= 'a' && c <= 'f') return c - 'a' + 10;
            if (c >= 'A' && c <= 'F') return c - 'A' + 10;
            return -1;
        }
        bool Read4Hex(uint32_t& cp)
        {
            if (m_pos + 4 > m_t.size()) return false;
            cp = 0;
            for (int k = 0; k < 4; k++)
            {
                int h = Hex(m_t[m_pos++]);
                if (h < 0) return false;
                cp = (cp << 4) | uint32_t(h);
            }
            return true;
        }
        static void AppendUtf8(std::string& out, uint32_t cp)
        {
            if (cp < 0x80) out += static_cast<char>(cp);
            else if (cp < 0x800) { out += static_cast<char>(0xC0 | (cp >> 6)); out += static_cast<char>(0x80 | (cp & 0x3F)); }
            else if (cp < 0x10000) { out += static_cast<char>(0xE0 | (cp >> 12)); out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F)); out += static_cast<char>(0x80 | (cp & 0x3F)); }
            else { out += static_cast<char>(0xF0 | (cp >> 18)); out += static_cast<char>(0x80 | ((cp >> 12) & 0x3F)); out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F)); out += static_cast<char>(0x80 | (cp & 0x3F)); }
        }
        bool ParseString(std::string& out)
        {
            m_pos++;   // "
            out.clear();
            while (m_pos < m_t.size())
            {
                unsigned char c = static_cast<unsigned char>(m_t[m_pos++]);
                if (c == '"') return true;
                if (c < 0x20) return false;   // ký tự điều khiển chưa thoát
                if (c != '\\') { out += static_cast<char>(c); continue; }
                if (m_pos >= m_t.size()) return false;
                char e = m_t[m_pos++];
                switch (e)
                {
                case '"': out += '"'; break;
                case '\\': out += '\\'; break;
                case '/': out += '/'; break;
                case 'b': out += '\b'; break;
                case 'f': out += '\f'; break;
                case 'n': out += '\n'; break;
                case 'r': out += '\r'; break;
                case 't': out += '\t'; break;
                case 'u':
                {
                    uint32_t cp;
                    if (!Read4Hex(cp)) return false;
                    if (cp >= 0xD800 && cp <= 0xDBFF)
                    {
                        uint32_t lo;
                        if (m_pos + 2 > m_t.size() || m_t[m_pos] != '\\' || m_t[m_pos + 1] != 'u') return false;
                        m_pos += 2;
                        if (!Read4Hex(lo) || lo < 0xDC00 || lo > 0xDFFF) return false;
                        cp = 0x10000 + ((cp - 0xD800) << 10) + (lo - 0xDC00);
                    }
                    else if (cp >= 0xDC00 && cp <= 0xDFFF)
                        return false;
                    AppendUtf8(out, cp);
                    break;
                }
                default: return false;
                }
            }
            return false;
        }
        bool ParseNumber(JsonValue& v)
        {
            v.type = JsonValue::Type::Number;
            bool neg = false;
            if (m_t[m_pos] == '-') { neg = true; m_pos++; }
            if (m_pos >= m_t.size() || m_t[m_pos] < '0' || m_t[m_pos] > '9') return false;
            if (m_t[m_pos] == '0' && m_pos + 1 < m_t.size() && m_t[m_pos + 1] >= '0' && m_t[m_pos + 1] <= '9') return false;
            bool overflow = false;
            uint64_t mag = 0;
            while (m_pos < m_t.size() && m_t[m_pos] >= '0' && m_t[m_pos] <= '9')
            {
                unsigned d = unsigned(m_t[m_pos++] - '0');
                if (mag > (UINT64_MAX - d) / 10) overflow = true;
                else mag = mag * 10 + d;
            }
            bool frac = false;
            if (m_pos < m_t.size() && m_t[m_pos] == '.')
            {
                frac = true; m_pos++;
                size_t s = m_pos;
                while (m_pos < m_t.size() && m_t[m_pos] >= '0' && m_t[m_pos] <= '9') m_pos++;
                if (m_pos == s) return false;
            }
            if (m_pos < m_t.size() && (m_t[m_pos] == 'e' || m_t[m_pos] == 'E'))
            {
                frac = true; m_pos++;
                if (m_pos < m_t.size() && (m_t[m_pos] == '+' || m_t[m_pos] == '-')) m_pos++;
                size_t s = m_pos;
                while (m_pos < m_t.size() && m_t[m_pos] >= '0' && m_t[m_pos] <= '9') m_pos++;
                if (m_pos == s) return false;
            }
            const uint64_t limit = neg ? uint64_t(INT64_MAX) + 1 : uint64_t(INT64_MAX);
            v.is_int = !frac && !overflow && mag <= limit;
            if (v.is_int)
                v.i = neg ? (mag == uint64_t(INT64_MAX) + 1 ? INT64_MIN : -int64_t(mag)) : int64_t(mag);
            return true;
        }
    };

    // ---------------------------------------------------------------------------------------------------------
    // URL đăng nhập và cổng quay về loopback
    // ---------------------------------------------------------------------------------------------------------
    inline std::string UrlEncode(const std::string& s)
    {
        static const char* hex = "0123456789ABCDEF";
        std::string out;
        for (unsigned char c : s)
        {
            if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-' || c == '.' || c == '_' || c == '~')
                out += static_cast<char>(c);
            else { out += '%'; out += hex[c >> 4]; out += hex[c & 15]; }
        }
        return out;
    }

    // Giải %XX; '+' giữ nguyên (callback OIDC không mã hoá khoảng trắng thành '+'). %XX hỏng => lỗi.
    inline bool UrlDecode(const std::string& s, std::string& out)
    {
        out.clear();
        for (size_t i = 0; i < s.size(); i++)
        {
            if (s[i] != '%') { out += s[i]; continue; }
            if (i + 2 >= s.size()) return false;
            auto hx = [](char c) -> int {
                if (c >= '0' && c <= '9') return c - '0';
                if (c >= 'a' && c <= 'f') return c - 'a' + 10;
                if (c >= 'A' && c <= 'F') return c - 'A' + 10;
                return -1;
            };
            int h = hx(s[i + 1]), l = hx(s[i + 2]);
            if (h < 0 || l < 0) return false;
            out += static_cast<char>(h * 16 + l);
            i += 2;
        }
        return true;
    }

    // redirect_uri so khớp chuỗi tuyệt đối phía máy chủ (AUTH mục 4): http://127.0.0.1:<cổng>/callback
    inline std::string RedirectUri(unsigned port)
    {
        return FORK_TSUDEV_LOOPBACK_PREFIX + std::to_string(port) + kCallbackPath;
    }

    struct AuthorizationRequest
    {
        std::string redirect_uri;
        std::string state;       // >= 128 bit ngẫu nhiên, chống CSRF trên cổng loopback
        std::string nonce;       // buộc id_token vào đúng lượt này
        std::string verifier;    // PKCE code_verifier (43 ký tự = 256 bit)
        std::string challenge;   // base64url(SHA-256(verifier))

        std::string Url(const std::string& endpoint = kAuthorizationEndpoint) const
        {
            const std::pair<const char*, std::string> q[] = {
                { "response_type", "code" },
                { "client_id", kClientId },
                { "redirect_uri", redirect_uri },
                { "scope", kScope },
                { "state", state },
                { "nonce", nonce },
                { "code_challenge", challenge },
                { "code_challenge_method", "S256" },
                { "prompt", "consent" },
            };
            std::string url = endpoint;
            char sep = '?';
            for (const auto& kv : q)
            {
                url += sep; url += kv.first; url += '='; url += UrlEncode(kv.second);
                sep = '&';
            }
            return url;
        }
        bool StateMatches(const std::string& received) const
        {
            return !state.empty() && ConstantTimeEquals(received, state);
        }
        // Thân POST /oidc/token (application/x-www-form-urlencoded), client công khai: không client_secret
        std::string TokenRequestBody(const std::string& code) const
        {
            return "grant_type=authorization_code&code=" + UrlEncode(code) + "&redirect_uri=" + UrlEncode(redirect_uri)
                + "&client_id=" + UrlEncode(kClientId) + "&code_verifier=" + UrlEncode(verifier);
        }
    };

    inline std::string RefreshRequestBody(const std::string& refresh_token)
    {
        return "grant_type=refresh_token&refresh_token=" + UrlEncode(refresh_token) + "&client_id=" + UrlEncode(kClientId);
    }

    // Kết quả đọc dòng đầu của yêu cầu HTTP trình duyệt gửi về cổng loopback.
    struct CallbackResult
    {
        bool ok = false;           // đúng "GET /callback?... HTTP/1.x" và query giải mã được
        std::string code, state, error, error_description, iss;
        bool has_iss = false;
        // RFC 9207: tsudev.com bật authorization_response_iss_parameter_supported => iss PHẢI có và đúng
        bool IssuerOk() const { return has_iss && iss == kIssuer; }
    };

    inline CallbackResult ParseCallbackRequestLine(const std::string& line)
    {
        CallbackResult r;
        if (line.size() > 8192 || line.compare(0, 4, "GET ") != 0)
            return r;
        size_t sp = line.find(' ', 4);
        if (sp == std::string::npos)
            return r;
        std::string target = line.substr(4, sp - 4);
        std::string version = line.substr(sp + 1);
        while (!version.empty() && (version.back() == '\r' || version.back() == '\n')) version.pop_back();
        if (version != "HTTP/1.1" && version != "HTTP/1.0")
            return r;
        size_t qm = target.find('?');
        std::string path = target.substr(0, qm);
        if (path != kCallbackPath || qm == std::string::npos)
            return r;
        std::string query = target.substr(qm + 1);
        size_t frag = query.find('#');
        if (frag != std::string::npos) query.resize(frag);
        std::map<std::string, std::string> seen;
        size_t pos = 0;
        while (pos <= query.size())
        {
            size_t amp = query.find('&', pos);
            if (amp == std::string::npos) amp = query.size();
            std::string pair = query.substr(pos, amp - pos);
            pos = amp + 1;
            if (pair.empty()) { if (amp == query.size()) break; continue; }
            size_t eq = pair.find('=');
            std::string k, v;
            if (!UrlDecode(pair.substr(0, eq), k)) return r;
            if (!UrlDecode(eq == std::string::npos ? std::string() : pair.substr(eq + 1), v)) return r;
            if (!seen.emplace(k, v).second) return r;   // tham số lặp (state=a&state=b) => từ chối
            if (amp == query.size()) break;
        }
        auto take = [&](const char* k, std::string& dst) { auto it = seen.find(k); if (it != seen.end()) dst = it->second; return it != seen.end(); };
        take("code", r.code);
        take("state", r.state);
        take("error", r.error);
        take("error_description", r.error_description);
        r.has_iss = take("iss", r.iss);
        r.ok = !r.code.empty() || !r.error.empty();
        return r;
    }

    // Mã cài đặt (AUTH 14.3): 128 bit ngẫu nhiên, base64url không đệm = đúng 22 ký tự.
    inline bool IsValidInstallationId(const std::string& id)
    {
        std::string raw;
        return id.size() == 22 && Base64UrlDecode(id, raw) && raw.size() == 16;
    }

    // ---------------------------------------------------------------------------------------------------------
    // JWKS và JWS (ES256 duy nhất)
    // ---------------------------------------------------------------------------------------------------------
    struct Jwk
    {
        std::string kid;
        std::string x, y;   // tọa độ P-256, đúng 32 byte mỗi trục
    };
    using JwkSet = std::map<std::string, Jwk>;

    // {"keys":[...]}: chỉ giữ khoá EC P-256 dùng để ký (alg ES256 nếu có); khoá loại khác bị bỏ qua thay vì làm
    // hỏng cả bộ; kid trùng => giữ khoá đầu. JSON hỏng/thiếu mảng keys => false.
    inline bool ParseJwks(const std::string& json, JwkSet& out)
    {
        out.clear();
        if (json.size() > kMaxTokenSize * 4)
            return false;
        JsonValue root;
        if (!JsonParser::Parse(json, root))
            return false;
        const JsonValue* keys = root.Get("keys");
        if (!keys || keys->type != JsonValue::Type::Array)
            return false;
        for (const auto& k : keys->arr)
        {
            std::string kty, crv, alg, use, kid, xs, ys;
            if (k.type != JsonValue::Type::Object) continue;
            if (!k.GetString("kty", kty) || kty != "EC" || !k.GetString("crv", crv) || crv != "P-256") continue;
            if (k.GetString("alg", alg) && alg != kAlgorithm) continue;
            if (k.GetString("use", use) && use != "sig") continue;
            if (!k.GetString("kid", kid) || kid.empty()) continue;
            Jwk j;
            j.kid = kid;
            if (!k.GetString("x", xs) || !k.GetString("y", ys)) continue;
            if (!Base64UrlDecode(xs, j.x) || !Base64UrlDecode(ys, j.y) || j.x.size() != 32 || j.y.size() != 32) continue;
            out.emplace(kid, std::move(j));
        }
        return true;
    }

    // Hàm kiểm chữ ký ES256: chữ ký JWS là R||S 64 byte (IEEE P1363), KHÔNG phải DER.
    using Es256Verifier = std::function<bool(const Jwk& key, const std::string& signing_input, const std::string& sig)>;

    enum class JwsError
    {
        None,
        Malformed,
        UnsupportedAlgorithm,
        WrongType,
        UnknownKey,
        BadSignature,
        WrongIssuer,
        WrongAudience,
        Expired,
        LifetimeTooLong,
        IssuedInFuture,
        WrongNonce,
        MissingSubject,
    };

    inline const char* JwsErrorName(JwsError e)
    {
        switch (e)
        {
        case JwsError::None: return "none";
        case JwsError::Malformed: return "malformed";
        case JwsError::UnsupportedAlgorithm: return "alg";
        case JwsError::WrongType: return "typ";
        case JwsError::UnknownKey: return "kid";
        case JwsError::BadSignature: return "signature";
        case JwsError::WrongIssuer: return "iss";
        case JwsError::WrongAudience: return "aud";
        case JwsError::Expired: return "exp";
        case JwsError::LifetimeTooLong: return "lifetime";
        case JwsError::IssuedInFuture: return "iat";
        case JwsError::WrongNonce: return "nonce";
        case JwsError::MissingSubject: return "sub";
        }
        return "?";
    }

    // Tách + kiểm header + chữ ký. required_typ rỗng = không đòi typ (id_token). Trả payload đã phân tích.
    inline JwsError VerifyCompactJws(const std::string& compact, const JwkSet& keys, const char* required_typ,
                                     const Es256Verifier& verify, JsonValue& payload)
    {
        if (compact.empty() || compact.size() > kMaxTokenSize || !verify)
            return JwsError::Malformed;
        size_t d1 = compact.find('.');
        size_t d2 = d1 == std::string::npos ? std::string::npos : compact.find('.', d1 + 1);
        if (d1 == std::string::npos || d2 == std::string::npos || compact.find('.', d2 + 1) != std::string::npos)
            return JwsError::Malformed;
        std::string h64 = compact.substr(0, d1), p64 = compact.substr(d1 + 1, d2 - d1 - 1), s64 = compact.substr(d2 + 1);
        if (h64.empty() || p64.empty() || s64.empty())
            return JwsError::Malformed;
        std::string hraw, praw, sig;
        if (!Base64UrlDecode(h64, hraw) || !Base64UrlDecode(p64, praw) || !Base64UrlDecode(s64, sig))
            return JwsError::Malformed;
        JsonValue header;
        if (!JsonParser::Parse(hraw, header) || header.type != JsonValue::Type::Object)
            return JwsError::Malformed;
        std::string alg, typ, kid;
        if (!header.GetString("alg", alg) || alg != kAlgorithm)
            return JwsError::UnsupportedAlgorithm;
        if (required_typ && *required_typ && (!header.GetString("typ", typ) || typ != required_typ))
            return JwsError::WrongType;
        // Phần mở rộng bắt buộc mà ta không hiểu (RFC 7515 4.1.11) và khoá nhúng/khoá từ xa => từ chối
        if (header.Has("crit") || header.Has("jku") || header.Has("jwk") || header.Has("x5u") || header.Has("x5c"))
            return JwsError::Malformed;
        if (!header.GetString("kid", kid))
            return JwsError::UnknownKey;
        auto it = keys.find(kid);
        if (it == keys.end())
            return JwsError::UnknownKey;
        if (sig.size() != 64 || !verify(it->second, h64 + "." + p64, sig))
            return JwsError::BadSignature;
        if (!JsonParser::Parse(praw, payload) || payload.type != JsonValue::Type::Object)
            return JwsError::Malformed;
        return JwsError::None;
    }

    inline bool AudienceContains(const JsonValue& p, const std::string& client_id, bool require_azp_if_many)
    {
        const JsonValue* aud = p.Get("aud");
        if (!aud) return false;
        if (aud->type == JsonValue::Type::String) return aud->s == client_id;
        if (aud->type != JsonValue::Type::Array) return false;
        bool found = false;
        for (const auto& a : aud->arr)
            if (a.type == JsonValue::Type::String && a.s == client_id) found = true;
        if (!found) return false;
        if (require_azp_if_many && aud->arr.size() > 1)
        {
            std::string azp;
            return p.GetString("azp", azp) && azp == client_id;
        }
        return true;
    }

    // Quyền lợi TsuDEV Pro đã KIỂM CHỮ KÝ (PRODUCT_POLICY 7.4)
    struct Entitlement
    {
        std::string sub;
        int64_t iat = 0;
        int64_t exp = 0;
        int64_t pro_until = 0;     // 0 = chưa từng có Pro
        std::string pro_source;    // play | appstore | web | ...
        bool donor = false;
        std::string badge = "none";
        int64_t devices_30d = 0;

        bool IsProAt(int64_t now) const { return pro_until > now; }
    };

    inline JwsError VerifyEntitlementClaims(const std::string& compact, const JwkSet& keys, const std::string& client_id,
                                            int64_t now, const Es256Verifier& verify, Entitlement& out)
    {
        JsonValue p;
        JwsError e = VerifyCompactJws(compact, keys, kEntitlementType, verify, p);
        if (e != JwsError::None)
            return e;
        std::string iss;
        if (!p.GetString("iss", iss) || iss != kIssuer) return JwsError::WrongIssuer;
        if (!AudienceContains(p, client_id, false)) return JwsError::WrongAudience;
        if (!p.GetInt("exp", out.exp) || !p.GetInt("iat", out.iat) || !p.GetString("sub", out.sub) || out.sub.empty())
            return JwsError::Malformed;
        if (now >= out.exp) return JwsError::Expired;
        if (out.exp - out.iat > kMaxEntitlementLifetime) return JwsError::LifetimeTooLong;
        int64_t v = 0;
        if (p.GetInt("pro_until", v) && v > 0) out.pro_until = v;
        if (p.GetInt("devices_30d", v) && v >= 0) out.devices_30d = v;
        p.GetString("pro_source", out.pro_source);
        const JsonValue* donor = p.Get("donor");
        out.donor = donor && donor->type == JsonValue::Type::Bool && donor->b;
        p.GetString("badge", out.badge);
        return JwsError::None;
    }

    // Lỗi bất kỳ => out trở về mặc định (miễn phí), không giữ claim đọc dở
    inline JwsError VerifyEntitlement(const std::string& compact, const JwkSet& keys, const std::string& client_id,
                                      int64_t now, const Es256Verifier& verify, Entitlement& out)
    {
        out = Entitlement();
        JwsError e = VerifyEntitlementClaims(compact, keys, client_id, now, verify, out);
        if (e != JwsError::None)
            out = Entitlement();
        return e;
    }

    // OIDC Core 3.1.3.7: alg ES256, kid trong JWKS, iss, aud (nhiều phần tử => azp), exp, iat không ở tương lai, nonce.
    inline JwsError ValidateIdToken(const std::string& id_token, const JwkSet& keys, const std::string& client_id,
                                    const std::string& nonce, int64_t now, const Es256Verifier& verify, std::string& sub)
    {
        sub.clear();
        JsonValue p;
        JwsError e = VerifyCompactJws(id_token, keys, nullptr, verify, p);
        if (e != JwsError::None)
            return e;
        std::string iss, n;
        int64_t exp = 0, iat = 0;
        if (!p.GetString("iss", iss) || iss != kIssuer) return JwsError::WrongIssuer;
        if (!AudienceContains(p, client_id, true)) return JwsError::WrongAudience;
        if (!p.GetInt("exp", exp) || !p.GetInt("iat", iat)) return JwsError::Malformed;
        if (now - kClockSkew >= exp) return JwsError::Expired;
        if (iat > now + kClockSkew) return JwsError::IssuedInFuture;
        if (nonce.empty() || !p.GetString("nonce", n) || !ConstantTimeEquals(n, nonce)) return JwsError::WrongNonce;
        if (!p.GetString("sub", sub) || sub.empty()) { sub.clear(); return JwsError::MissingSubject; }
        return JwsError::None;
    }

    // Phản hồi token: {"access_token","id_token","refresh_token"?,"token_type":"Bearer","expires_in"}
    struct TokenResponse
    {
        std::string access_token, id_token, refresh_token, error;
        int64_t expires_in = 0;
    };
    inline bool ParseTokenResponse(const std::string& json, TokenResponse& out)
    {
        out = TokenResponse();
        JsonValue v;
        if (!JsonParser::Parse(json, v) || v.type != JsonValue::Type::Object)
            return false;
        if (v.GetString("error", out.error))
            return false;
        std::string type;
        if (!v.GetString("access_token", out.access_token) || out.access_token.empty()) return false;
        if (!v.GetString("token_type", type) || (type != "Bearer" && type != "bearer")) return false;
        v.GetString("id_token", out.id_token);
        v.GetString("refresh_token", out.refresh_token);
        v.GetInt("expires_in", out.expires_in);
        return true;
    }

    // Thân {"jws": "<compact>"} của GET /api/me/entitlements
    inline bool ParseEntitlementResponse(const std::string& json, std::string& jws)
    {
        JsonValue v;
        return JsonParser::Parse(json, v) && v.GetString("jws", jws) && !jws.empty();
    }

#ifdef _WIN32
    // ---------------------------------------------------------------------------------------------------------
    // Mật mã của nền tảng (BCrypt) - chỉ Windows
    // ---------------------------------------------------------------------------------------------------------
    inline bool RandomBytes(void* buf, size_t size)
    {
        return BCRYPT_SUCCESS(BCryptGenRandom(nullptr, static_cast<PUCHAR>(buf), static_cast<ULONG>(size), BCRYPT_USE_SYSTEM_PREFERRED_RNG));
    }

    inline bool Sha256(const std::string& data, unsigned char digest[32])
    {
        BCRYPT_ALG_HANDLE alg = nullptr;
        BCRYPT_HASH_HANDLE hash = nullptr;
        bool ok = BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&alg, BCRYPT_SHA256_ALGORITHM, nullptr, 0))
            && BCRYPT_SUCCESS(BCryptCreateHash(alg, &hash, nullptr, 0, nullptr, 0, 0))
            && BCRYPT_SUCCESS(BCryptHashData(hash, reinterpret_cast<PUCHAR>(const_cast<char*>(data.data())), static_cast<ULONG>(data.size()), 0))
            && BCRYPT_SUCCESS(BCryptFinishHash(hash, digest, 32, 0));
        if (hash) BCryptDestroyHash(hash);
        if (alg) BCryptCloseAlgorithmProvider(alg, 0);
        return ok;
    }

    inline std::string PkceChallenge(const std::string& verifier)
    {
        unsigned char d[32];
        if (!Sha256(verifier, d)) return std::string();
        return Base64UrlEncode(d, sizeof(d));
    }

    // Chuỗi ngẫu nhiên n byte, base64url (state/nonce/verifier: 32 byte = 256 bit)
    inline std::string RandomToken(size_t n = 32)
    {
        std::vector<unsigned char> b(n);
        if (!RandomBytes(b.data(), b.size())) return std::string();
        return Base64UrlEncode(b.data(), b.size());
    }

    // Mã cài đặt mới (AUTH 14.3): 128 bit từ bộ sinh số ngẫu nhiên mật mã, KHÔNG suy ra từ phần cứng
    inline std::string NewInstallationId() { return RandomToken(16); }

    // Lỗi sinh số ngẫu nhiên => trả yêu cầu rỗng (state rỗng); người gọi phải kiểm IsUsable()
    inline AuthorizationRequest NewAuthorizationRequest(unsigned port)
    {
        AuthorizationRequest r;
        r.redirect_uri = RedirectUri(port);
        r.state = RandomToken();
        r.nonce = RandomToken();
        r.verifier = RandomToken();
        r.challenge = r.verifier.empty() ? std::string() : PkceChallenge(r.verifier);
        if (r.state.empty() || r.nonce.empty() || r.challenge.empty())
            r = AuthorizationRequest();
        return r;
    }

    // ES256 bằng BCrypt: khoá công khai BCRYPT_ECCPUBLIC_BLOB (P-256), chữ ký R||S 64 byte như JWS.
    // BCryptImportKeyPair từ chối điểm không nằm trên đường cong.
    inline bool VerifyEs256(const Jwk& key, const std::string& signing_input, const std::string& sig)
    {
        if (key.x.size() != 32 || key.y.size() != 32 || sig.size() != 64)
            return false;
        unsigned char digest[32];
        if (!Sha256(signing_input, digest))
            return false;
        std::vector<unsigned char> blob(sizeof(BCRYPT_ECCKEY_BLOB) + 64);
        auto* hdr = reinterpret_cast<BCRYPT_ECCKEY_BLOB*>(blob.data());
        hdr->dwMagic = BCRYPT_ECDSA_PUBLIC_P256_MAGIC;
        hdr->cbKey = 32;
        std::memcpy(blob.data() + sizeof(BCRYPT_ECCKEY_BLOB), key.x.data(), 32);
        std::memcpy(blob.data() + sizeof(BCRYPT_ECCKEY_BLOB) + 32, key.y.data(), 32);
        BCRYPT_ALG_HANDLE alg = nullptr;
        BCRYPT_KEY_HANDLE hkey = nullptr;
        bool ok = false;
        if (BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&alg, BCRYPT_ECDSA_P256_ALGORITHM, nullptr, 0))
            && BCRYPT_SUCCESS(BCryptImportKeyPair(alg, nullptr, BCRYPT_ECCPUBLIC_BLOB, &hkey, blob.data(), static_cast<ULONG>(blob.size()), 0)))
        {
            ok = BCRYPT_SUCCESS(BCryptVerifySignature(hkey, nullptr, digest, sizeof(digest),
                reinterpret_cast<PUCHAR>(const_cast<char*>(sig.data())), static_cast<ULONG>(sig.size()), 0));
        }
        if (hkey) BCryptDestroyKey(hkey);
        if (alg) BCryptCloseAlgorithmProvider(alg, 0);
        return ok;
    }
#endif
}
