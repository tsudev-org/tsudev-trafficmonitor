// Kiểm thử logic đăng nhập tsudev (OIDC + PKCE) và JWS quyền lợi TsuDEV Pro (tsutramon/ProAuth.h, P9).
// Bộ test KHÔNG gọi mạng, không gọi tsudev.com. Build và chạy trong CI (build.yml, job "Kiểm thử C++"); chạy tay:
//   cl /nologo /std:c++17 /EHsc /W4 /WX /utf-8 /sdl /I tsutramon tests\ProAuthTests.cpp /Fe:ProAuthTests.exe
//   g++ -std=c++17 -Wall -Wextra -Werror -fsanitize=address,undefined -I tsutramon tests/ProAuthTests.cpp
// Phần mật mã thật (PKCE RFC 7636, ES256 ký bằng khoá BCrypt sinh trong test) chỉ chạy trên Windows.

#include "ProAuth.h"
#include <iostream>

using namespace ProAuth;

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

static const int64_t T0 = 1'800'000'000;

// ---------------------------------------------------------------------------------------------------------------
static void TestBase64Url()
{
    // RFC 4648 mục 10 (đổi sang bảng url, bỏ đệm)
    CHECK(Base64UrlEncode("") == "");
    CHECK(Base64UrlEncode("f") == "Zg");
    CHECK(Base64UrlEncode("fo") == "Zm8");
    CHECK(Base64UrlEncode("foo") == "Zm9v");
    CHECK(Base64UrlEncode("foob") == "Zm9vYg");
    CHECK(Base64UrlEncode("fooba") == "Zm9vYmE");
    CHECK(Base64UrlEncode("foobar") == "Zm9vYmFy");
    const unsigned char hi[] = { 0xFB, 0xFF, 0xBF };
    CHECK(Base64UrlEncode(hi, 3) == "-_-_");
    std::string out;
    CHECK(Base64UrlDecode("Zm9vYmE", out) && out == "fooba");
    CHECK(Base64UrlDecode("-_-_", out) && out == std::string(reinterpret_cast<const char*>(hi), 3));
    CHECK(!Base64UrlDecode("Zm9vYg==", out));   // đệm
    CHECK(!Base64UrlDecode("Zm9+", out));       // bảng chuẩn, không phải url
    CHECK(!Base64UrlDecode("Z", out));          // độ dài % 4 == 1
    CHECK(!Base64UrlDecode("Zh", out));         // bit thừa khác 0 (không chuẩn tắc)
    CHECK(Base64UrlDecode("Zg", out) && out == "f");
}

static void TestConstantTimeEquals()
{
    CHECK(ConstantTimeEquals("abc", "abc"));
    CHECK(!ConstantTimeEquals("abc", "abd"));
    CHECK(!ConstantTimeEquals("abc", "ab"));
    CHECK(!ConstantTimeEquals("", "a"));
    CHECK(ConstantTimeEquals("", ""));
}

static void TestJson()
{
    JsonValue v;
    CHECK(JsonParser::Parse(R"({"a":1,"b":"x","c":[true,false,null],"d":{"e":-5}})", v));
    int64_t n = 0;
    std::string s;
    CHECK(v.GetInt("a", n) && n == 1);
    CHECK(v.GetString("b", s) && s == "x");
    CHECK(v.Get("c") && v.Get("c")->arr.size() == 3);
    CHECK(v.Get("d") && v.Get("d")->GetInt("e", n) && n == -5);
    CHECK(!v.GetInt("b", n));        // sai kiểu
    CHECK(!v.GetString("a", s));
    // Thoát ký tự, \u có cặp thay thế => UTF-8
    // Chuỗi thường, KHÔNG raw string: MSVC dịch \uXXXX cả trong R"(...)" (g++ thì không) => lỗi C3850 ở CI
    const std::string esc = "{\"s\":\"a\\\"b\\\\c\\/\\n\\u1EC7\\uD83D\\uDE00\"}";
    CHECK(JsonParser::Parse(esc, v) && v.GetString("s", s)
          && s == "a\"b\\c/\n\xE1\xBB\x87\xF0\x9F\x98\x80");
    // Số: nguyên int64, biên, thập phân/mũ không phải nguyên
    CHECK(JsonParser::Parse(R"({"n":9223372036854775807,"m":-9223372036854775808})", v));
    CHECK(v.GetInt("n", n) && n == INT64_MAX);
    CHECK(v.GetInt("m", n) && n == INT64_MIN);
    CHECK(JsonParser::Parse(R"({"n":9223372036854775808})", v) && !v.GetInt("n", n));
    CHECK(JsonParser::Parse(R"({"n":1.5,"e":1e3})", v) && !v.GetInt("n", n) && !v.GetInt("e", n));
    CHECK(JsonParser::Parse("  [ ] ", v) && v.type == JsonValue::Type::Array);
    // Từ chối
    const char* bad[] = {
        "", "{", "}", "{\"a\":1,}", "[1,]", "{\"a\" 1}", "{'a':1}", "{\"a\":01}", "{\"a\":-}", "{\"a\":1.}",
        "{\"a\":tru}", "{\"a\":1} x", "{\"a\":\"\x01\"}", "{\"a\":\"\\x\"}", "{\"a\":\"\\uD83D\"}",
        "{\"a\":\"\\uDE00\"}", "{\"a\":1,\"a\":2}", "{\"a\":\"abc}", "nul", "[[[[[[[[[[1]]]]]]]]]]",
    };
    for (const char* b : bad)
    {
        JsonValue x;
        if (JsonParser::Parse(b, x))
        {
            g_failed++;
            std::cerr << "THAT BAI: JSON hong duoc nhan: " << b << "\n";
        }
        else
            g_passed++;
    }
    // Giới hạn số phần tử
    std::string big = "[";
    for (int i = 0; i < 2000; i++) big += (i ? ",1" : "1");
    big += "]";
    CHECK(!JsonParser::Parse(big, v));
}

static void TestAuthorizationUrl()
{
    AuthorizationRequest r;
    r.redirect_uri = RedirectUri(53123);
    r.state = "st-1";
    r.nonce = "no_2";
    r.verifier = "dBjftJeZ4CVP-mB92K27uhbUJU1p1r_wW1gFWFOEjXk";
    r.challenge = "E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cM";
    CHECK(r.redirect_uri == "http://127.0.0.1:53123/callback");
    const std::string url = r.Url();
    CHECK(url ==
          "https://tsudev.com/oidc/auth?response_type=code&client_id=tsutramon"
          "&redirect_uri=http%3A%2F%2F127.0.0.1%3A53123%2Fcallback"
          "&scope=openid%20offline_access%20tsudev.entitlements&state=st-1&nonce=no_2"
          "&code_challenge=E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cM&code_challenge_method=S256&prompt=consent");
    CHECK(r.StateMatches("st-1"));
    CHECK(!r.StateMatches("st-2"));
    CHECK(!r.StateMatches(""));
    AuthorizationRequest rong;
    CHECK(!rong.StateMatches(""));   // yêu cầu hỏng (state rỗng) không bao giờ khớp
    CHECK(r.TokenRequestBody("c/d+e") ==
          "grant_type=authorization_code&code=c%2Fd%2Be&redirect_uri=http%3A%2F%2F127.0.0.1%3A53123%2Fcallback"
          "&client_id=tsutramon&code_verifier=dBjftJeZ4CVP-mB92K27uhbUJU1p1r_wW1gFWFOEjXk");
    CHECK(RefreshRequestBody("r t") == "grant_type=refresh_token&refresh_token=r%20t&client_id=tsutramon");
    CHECK(ProPageUrl() == "https://tsudev.com/pro?app=tsutramon");
    // idp_hint (AUTH 13.2): chỉ google/github được nối vào cuối; giá trị lạ hoặc có ký tự đặc biệt bị bỏ hẳn
    CHECK(IsKnownIdpHint("google") && IsKnownIdpHint("github"));
    CHECK(!IsKnownIdpHint("") && !IsKnownIdpHint("apple") && !IsKnownIdpHint("Google") && !IsKnownIdpHint("google&prompt=none"));
    r.idp_hint = "github";
    CHECK(r.Url() == url + "&idp_hint=github");
    r.idp_hint = "google";
    CHECK(r.Url() == url + "&idp_hint=google");
    r.idp_hint = "google&prompt=none";
    CHECK(r.Url() == url);
    r.idp_hint = "apple";
    CHECK(r.Url() == url);
}

static void TestCallback()
{
    auto r = ParseCallbackRequestLine("GET /callback?code=abc%2Fd&state=xyz&iss=https%3A%2F%2Ftsudev.com%2Foidc HTTP/1.1\r\n");
    CHECK(r.ok && r.code == "abc/d" && r.state == "xyz" && r.IssuerOk());
    r = ParseCallbackRequestLine("GET /callback?error=access_denied&error_description=Ng%C6%B0%E1%BB%9Di%20d%C3%B9ng%20h%E1%BB%A7y&state=xyz&iss=https%3A%2F%2Ftsudev.com%2Foidc HTTP/1.1");
    CHECK(r.ok && r.code.empty() && r.error == "access_denied" && r.error_description == "Người dùng hủy");
    // iss thiếu hoặc sai => IssuerOk false (RFC 9207, chống trộn IdP)
    r = ParseCallbackRequestLine("GET /callback?code=a&state=b HTTP/1.1");
    CHECK(r.ok && !r.IssuerOk());
    r = ParseCallbackRequestLine("GET /callback?code=a&state=b&iss=https%3A%2F%2Fevil.example%2Foidc HTTP/1.1");
    CHECK(r.ok && !r.IssuerOk());
    // Từ chối: sai đường dẫn, sai phương thức, tham số lặp, %XX hỏng, không có code lẫn error, sai phiên bản HTTP
    CHECK(!ParseCallbackRequestLine("GET /callbackx?code=a&state=b HTTP/1.1").ok);
    CHECK(!ParseCallbackRequestLine("GET /favicon.ico HTTP/1.1").ok);
    CHECK(!ParseCallbackRequestLine("POST /callback?code=a&state=b HTTP/1.1").ok);
    CHECK(!ParseCallbackRequestLine("GET /callback?code=a&state=b&state=c HTTP/1.1").ok);
    CHECK(!ParseCallbackRequestLine("GET /callback?code=a%G1&state=b HTTP/1.1").ok);
    CHECK(!ParseCallbackRequestLine("GET /callback?state=b HTTP/1.1").ok);
    CHECK(!ParseCallbackRequestLine("GET /callback?code=a HTTP/2").ok);
    CHECK(!ParseCallbackRequestLine("GET /callback?code=a").ok);
    CHECK(!ParseCallbackRequestLine("").ok);
    CHECK(!ParseCallbackRequestLine("GET /callback?code=" + std::string(9000, 'a') + " HTTP/1.1").ok);
}

static void TestInstallationId()
{
    CHECK(IsValidInstallationId("AAAAAAAAAAAAAAAAAAAAAA"));
    CHECK(IsValidInstallationId("q83vEjRWeJA_-Q8eLS0ung"));
    CHECK(!IsValidInstallationId("AAAAAAAAAAAAAAAAAAAAA"));     // 21
    CHECK(!IsValidInstallationId("AAAAAAAAAAAAAAAAAAAAAAA"));   // 23
    CHECK(!IsValidInstallationId("AAAAAAAAAAAAAAAAAAAA+/"));    // bảng chuẩn
    CHECK(!IsValidInstallationId("AAAAAAAAAAAAAAAAAAAAAB"));    // bit thừa khác 0
}

// ---------------------------------------------------------------------------------------------------------------
// JWS với bộ kiểm chữ ký giả: chữ ký hợp lệ khi và chỉ khi nó là 64 byte 'S' - tách đối chiếu claim khỏi mật mã.
static const std::string kSigOk(64, 'S');
static bool FakeVerify(const Jwk& k, const std::string&, const std::string& sig) { return k.kid == "k1" && sig == kSigOk; }

static JwkSet Keys()
{
    JwkSet s;
    Jwk k;
    k.kid = "k1";
    k.x.assign(32, 'x');
    k.y.assign(32, 'y');
    s.emplace("k1", k);
    return s;
}

static std::string Jws(const std::string& header, const std::string& payload, const std::string& sig = kSigOk)
{
    return Base64UrlEncode(header) + "." + Base64UrlEncode(payload) + "." + Base64UrlEncode(sig);
}

static const char* kEntHeader = R"({"alg":"ES256","typ":"tsu-ent+jwt","kid":"k1"})";

static std::string EntPayload(int64_t iat, int64_t exp, const std::string& extra = "",
                              const std::string& aud = "\"tsutramon\"", const std::string& iss = "https://tsudev.com/oidc")
{
    return "{\"iss\":\"" + iss + "\",\"aud\":" + aud + ",\"sub\":\"u1\",\"iat\":" + std::to_string(iat) + ",\"exp\":"
        + std::to_string(exp) + extra + "}";
}

static void TestJwks()
{
    const std::string x = Base64UrlEncode(std::string(32, 'x')), y = Base64UrlEncode(std::string(32, 'y'));
    const std::string json = R"({"keys":[
        {"kty":"RSA","kid":"r","n":"AQAB","e":"AQAB"},
        {"kty":"EC","crv":"P-384","kid":"p384","x":")" + x + R"(","y":")" + y + R"("},
        {"kty":"EC","crv":"P-256","kid":"enc","use":"enc","x":")" + x + R"(","y":")" + y + R"("},
        {"kty":"EC","crv":"P-256","kid":"short","x":"AAAA","y":")" + y + R"("},
        {"kty":"EC","crv":"P-256","kid":"k1","alg":"ES256","use":"sig","x":")" + x + R"(","y":")" + y + R"("},
        {"kty":"EC","crv":"P-256","kid":"k2","x":")" + x + R"(","y":")" + y + R"("}
    ]})";
    JwkSet keys;
    CHECK(ParseJwks(json, keys));
    CHECK(keys.size() == 2 && keys.count("k1") == 1 && keys.count("k2") == 1);
    CHECK(!ParseJwks("{}", keys));
    CHECK(!ParseJwks("{\"keys\":{}}", keys));
    CHECK(!ParseJwks("không phải json", keys));
    CHECK(ParseJwks("{\"keys\":[]}", keys) && keys.empty());
}

static void TestEntitlement()
{
    const JwkSet keys = Keys();
    Entitlement e;
    // Hợp lệ, đủ trường
    CHECK(VerifyEntitlement(Jws(kEntHeader, EntPayload(T0, T0 + 7 * 86400,
        R"(,"pro_until":1900000000,"pro_source":"web","donor":true,"badge":"gold","devices_30d":2)")),
        keys, kClientId, T0 + 10, FakeVerify, e) == JwsError::None);
    CHECK(e.sub == "u1" && e.pro_until == 1'900'000'000 && e.pro_source == "web" && e.donor && e.badge == "gold" && e.devices_30d == 2);
    CHECK(e.IsProAt(T0) && !e.IsProAt(1'900'000'000));
    // Thiếu trường tuỳ chọn => mặc định miễn phí
    CHECK(VerifyEntitlement(Jws(kEntHeader, EntPayload(T0, T0 + 3600)), keys, kClientId, T0, FakeVerify, e) == JwsError::None);
    CHECK(e.pro_until == 0 && !e.donor && e.badge == "none" && !e.IsProAt(T0));
    // aud dạng mảng
    CHECK(VerifyEntitlement(Jws(kEntHeader, EntPayload(T0, T0 + 3600, "", R"(["khac","tsutramon"])")), keys, kClientId, T0, FakeVerify, e) == JwsError::None);

    // Header
    CHECK(VerifyEntitlement(Jws(R"({"alg":"none","typ":"tsu-ent+jwt","kid":"k1"})", EntPayload(T0, T0 + 60)), keys, kClientId, T0, FakeVerify, e) == JwsError::UnsupportedAlgorithm);
    CHECK(VerifyEntitlement(Jws(R"({"alg":"HS256","typ":"tsu-ent+jwt","kid":"k1"})", EntPayload(T0, T0 + 60)), keys, kClientId, T0, FakeVerify, e) == JwsError::UnsupportedAlgorithm);
    CHECK(VerifyEntitlement(Jws(R"({"alg":"ES256","typ":"JWT","kid":"k1"})", EntPayload(T0, T0 + 60)), keys, kClientId, T0, FakeVerify, e) == JwsError::WrongType);
    CHECK(VerifyEntitlement(Jws(R"({"alg":"ES256","typ":"tsu-ent+jwt"})", EntPayload(T0, T0 + 60)), keys, kClientId, T0, FakeVerify, e) == JwsError::UnknownKey);
    CHECK(VerifyEntitlement(Jws(R"({"alg":"ES256","typ":"tsu-ent+jwt","kid":"k9"})", EntPayload(T0, T0 + 60)), keys, kClientId, T0, FakeVerify, e) == JwsError::UnknownKey);
    CHECK(VerifyEntitlement(Jws(R"({"alg":"ES256","typ":"tsu-ent+jwt","kid":"k1","crit":["x"]})", EntPayload(T0, T0 + 60)), keys, kClientId, T0, FakeVerify, e) == JwsError::Malformed);
    CHECK(VerifyEntitlement(Jws(R"({"alg":"ES256","typ":"tsu-ent+jwt","kid":"k1","jku":"https://evil/jwks"})", EntPayload(T0, T0 + 60)), keys, kClientId, T0, FakeVerify, e) == JwsError::Malformed);
    CHECK(VerifyEntitlement(Jws(R"({"alg":"ES256","typ":"tsu-ent+jwt","kid":"k1","jwk":{}})", EntPayload(T0, T0 + 60)), keys, kClientId, T0, FakeVerify, e) == JwsError::Malformed);
    // Chữ ký
    CHECK(VerifyEntitlement(Jws(kEntHeader, EntPayload(T0, T0 + 60), std::string(64, 'X')), keys, kClientId, T0, FakeVerify, e) == JwsError::BadSignature);
    CHECK(VerifyEntitlement(Jws(kEntHeader, EntPayload(T0, T0 + 60), std::string(70, 'S')), keys, kClientId, T0, FakeVerify, e) == JwsError::BadSignature);
    // Claim
    CHECK(VerifyEntitlement(Jws(kEntHeader, EntPayload(T0, T0 + 60, "", "\"tsutramon\"", "https://evil.example/oidc")), keys, kClientId, T0, FakeVerify, e) == JwsError::WrongIssuer);
    CHECK(VerifyEntitlement(Jws(kEntHeader, EntPayload(T0, T0 + 60, "", "\"tsuswico\"")), keys, kClientId, T0, FakeVerify, e) == JwsError::WrongAudience);
    CHECK(VerifyEntitlement(Jws(kEntHeader, EntPayload(T0, T0 + 60, "", "[]")), keys, kClientId, T0, FakeVerify, e) == JwsError::WrongAudience);
    CHECK(VerifyEntitlement(Jws(kEntHeader, EntPayload(T0, T0 + 60)), keys, kClientId, T0 + 60, FakeVerify, e) == JwsError::Expired);
    CHECK(VerifyEntitlement(Jws(kEntHeader, EntPayload(T0, T0 + 7 * 86400 + 1)), keys, kClientId, T0, FakeVerify, e) == JwsError::LifetimeTooLong);
    CHECK(VerifyEntitlement(Jws(kEntHeader, R"({"iss":"https://tsudev.com/oidc","aud":"tsutramon","sub":"u1","exp":1800000060})"), keys, kClientId, T0, FakeVerify, e) == JwsError::Malformed);
    CHECK(VerifyEntitlement(Jws(kEntHeader, R"({"iss":"https://tsudev.com/oidc","aud":"tsutramon","sub":"u1","iat":"1800000000","exp":1800000060})"), keys, kClientId, T0, FakeVerify, e) == JwsError::Malformed);
    // Khi lỗi (kể cả lỗi ở claim sau khi đã đọc sub/pro_until), đầu ra không mang quyền lợi nào
    CHECK(VerifyEntitlement(Jws(kEntHeader, EntPayload(T0, T0 + 7 * 86400 + 1, R"(,"pro_until":1900000000)")), keys, kClientId, T0, FakeVerify, e) == JwsError::LifetimeTooLong);
    CHECK(e.sub.empty() && e.pro_until == 0 && !e.IsProAt(T0));
    // Cấu trúc
    CHECK(VerifyEntitlement("", keys, kClientId, T0, FakeVerify, e) == JwsError::Malformed);
    CHECK(VerifyEntitlement("a.b", keys, kClientId, T0, FakeVerify, e) == JwsError::Malformed);
    CHECK(VerifyEntitlement("a.b.c.d", keys, kClientId, T0, FakeVerify, e) == JwsError::Malformed);
    CHECK(VerifyEntitlement(".." , keys, kClientId, T0, FakeVerify, e) == JwsError::Malformed);
    CHECK(VerifyEntitlement(Jws(kEntHeader, "[1]"), keys, kClientId, T0, FakeVerify, e) == JwsError::Malformed);
    CHECK(VerifyEntitlement(std::string(kMaxTokenSize + 1, 'a'), keys, kClientId, T0, FakeVerify, e) == JwsError::Malformed);
    CHECK(VerifyEntitlement(Jws(kEntHeader, EntPayload(T0, T0 + 60)), keys, kClientId, T0, Es256Verifier(), e) == JwsError::Malformed);
}

static void TestIdToken()
{
    const JwkSet keys = Keys();
    const std::string h = R"({"alg":"ES256","kid":"k1"})";
    auto tok = [&](const std::string& extra, const std::string& aud = "\"tsutramon\"", int64_t iat = T0, int64_t exp = T0 + 3600) {
        return Jws(h, "{\"iss\":\"https://tsudev.com/oidc\",\"aud\":" + aud + ",\"sub\":\"u1\",\"iat\":" + std::to_string(iat)
            + ",\"exp\":" + std::to_string(exp) + extra + "}");
    };
    std::string sub;
    CHECK(ValidateIdToken(tok(",\"nonce\":\"n1\""), keys, kClientId, "n1", T0, FakeVerify, sub) == JwsError::None && sub == "u1");
    CHECK(ValidateIdToken(tok(",\"nonce\":\"n2\""), keys, kClientId, "n1", T0, FakeVerify, sub) == JwsError::WrongNonce && sub.empty());
    CHECK(ValidateIdToken(tok(""), keys, kClientId, "n1", T0, FakeVerify, sub) == JwsError::WrongNonce);
    CHECK(ValidateIdToken(tok(",\"nonce\":\"\""), keys, kClientId, "", T0, FakeVerify, sub) == JwsError::WrongNonce);
    // aud nhiều phần tử => azp bắt buộc = client_id
    CHECK(ValidateIdToken(tok(",\"nonce\":\"n1\"", R"(["tsutramon","khac"])"), keys, kClientId, "n1", T0, FakeVerify, sub) == JwsError::WrongAudience);
    CHECK(ValidateIdToken(tok(",\"nonce\":\"n1\",\"azp\":\"tsutramon\"", R"(["tsutramon","khac"])"), keys, kClientId, "n1", T0, FakeVerify, sub) == JwsError::None);
    // Lệch đồng hồ 5 phút
    CHECK(ValidateIdToken(tok(",\"nonce\":\"n1\"", "\"tsutramon\"", T0, T0 + 60), keys, kClientId, "n1", T0 + 60 + 299, FakeVerify, sub) == JwsError::None);
    CHECK(ValidateIdToken(tok(",\"nonce\":\"n1\"", "\"tsutramon\"", T0, T0 + 60), keys, kClientId, "n1", T0 + 60 + 300, FakeVerify, sub) == JwsError::Expired);
    CHECK(ValidateIdToken(tok(",\"nonce\":\"n1\"", "\"tsutramon\"", T0 + 301, T0 + 3600), keys, kClientId, "n1", T0, FakeVerify, sub) == JwsError::IssuedInFuture);
    // id_token không cần typ nhưng vẫn chỉ ES256
    CHECK(ValidateIdToken(Jws(R"({"alg":"RS256","kid":"k1"})", "{}"), keys, kClientId, "n1", T0, FakeVerify, sub) == JwsError::UnsupportedAlgorithm);
    // sub rỗng
    CHECK(ValidateIdToken(Jws(h, R"({"iss":"https://tsudev.com/oidc","aud":"tsutramon","sub":"","iat":1800000000,"exp":1800003600,"nonce":"n1"})"),
                          keys, kClientId, "n1", T0, FakeVerify, sub) == JwsError::MissingSubject);
}

static void TestResponses()
{
    TokenResponse t;
    CHECK(ParseTokenResponse(R"({"access_token":"at","id_token":"it","refresh_token":"rt","token_type":"Bearer","expires_in":3600,"scope":"openid"})", t));
    CHECK(t.access_token == "at" && t.id_token == "it" && t.refresh_token == "rt" && t.expires_in == 3600);
    CHECK(!ParseTokenResponse(R"({"error":"invalid_grant","error_description":"x"})", t) && t.error == "invalid_grant");
    CHECK(!ParseTokenResponse(R"({"access_token":"at","token_type":"DPoP"})", t));
    CHECK(!ParseTokenResponse(R"({"token_type":"Bearer"})", t));
    CHECK(!ParseTokenResponse("<html>", t));
    std::string jws;
    CHECK(ParseEntitlementResponse(R"({"jws":"a.b.c"})", jws) && jws == "a.b.c");
    CHECK(!ParseEntitlementResponse(R"({"jws":""})", jws));
    CHECK(!ParseEntitlementResponse(R"({"error":"invalid_token"})", jws));
}

#ifdef _WIN32
// ---------------------------------------------------------------------------------------------------------------
// Mật mã thật bằng BCrypt
static void TestPkceRfc7636()
{
    // RFC 7636 phụ lục B
    CHECK(PkceChallenge("dBjftJeZ4CVP-mB92K27uhbUJU1p1r_wW1gFWFOEjXk") == "E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cM");
    auto r = NewAuthorizationRequest(50000);
    CHECK(r.state.size() == 43 && r.nonce.size() == 43 && r.verifier.size() == 43);
    CHECK(r.state != r.nonce && r.nonce != r.verifier);
    CHECK(r.challenge == PkceChallenge(r.verifier));
    CHECK(r.redirect_uri == "http://127.0.0.1:50000/callback");
    CHECK(r.idp_hint.empty());
    CHECK(NewAuthorizationRequest(50000, "github").idp_hint == "github");
    CHECK(NewAuthorizationRequest(50000, "khong-ro").idp_hint.empty());
    const std::string id1 = NewInstallationId(), id2 = NewInstallationId();
    CHECK(IsValidInstallationId(id1) && IsValidInstallationId(id2) && id1 != id2);
}

// Ký ES256 bằng khoá P-256 sinh tại chỗ => VerifyEs256/VerifyEntitlement thật phải nhận; đổi 1 bit => từ chối.
static void TestEs256Real()
{
    BCRYPT_ALG_HANDLE alg = nullptr;
    BCRYPT_KEY_HANDLE key = nullptr;
    bool ok = BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&alg, BCRYPT_ECDSA_P256_ALGORITHM, nullptr, 0))
        && BCRYPT_SUCCESS(BCryptGenerateKeyPair(alg, &key, 256, 0))
        && BCRYPT_SUCCESS(BCryptFinalizeKeyPair(key, 0));
    CHECK(ok);
    if (!ok)
        return;
    std::vector<unsigned char> blob(sizeof(BCRYPT_ECCKEY_BLOB) + 64);
    ULONG got = 0;
    CHECK(BCRYPT_SUCCESS(BCryptExportKey(key, nullptr, BCRYPT_ECCPUBLIC_BLOB, blob.data(), static_cast<ULONG>(blob.size()), &got, 0)));
    Jwk jwk;
    jwk.kid = "real";
    jwk.x.assign(reinterpret_cast<const char*>(blob.data()) + sizeof(BCRYPT_ECCKEY_BLOB), 32);
    jwk.y.assign(reinterpret_cast<const char*>(blob.data()) + sizeof(BCRYPT_ECCKEY_BLOB) + 32, 32);
    JwkSet keys;
    keys.emplace("real", jwk);

    auto sign = [&](const std::string& input) {
        unsigned char d[32];
        Sha256(input, d);
        unsigned char sig[64];
        ULONG n = 0;
        BCryptSignHash(key, nullptr, d, 32, sig, 64, &n, 0);
        return std::string(reinterpret_cast<char*>(sig), n);
    };
    const std::string h64 = Base64UrlEncode(std::string(R"({"alg":"ES256","typ":"tsu-ent+jwt","kid":"real"})"));
    const std::string p64 = Base64UrlEncode(EntPayload(T0, T0 + 86400, R"(,"pro_until":1900000000)"));
    const std::string sig = sign(h64 + "." + p64);
    CHECK(sig.size() == 64);
    Entitlement e;
    CHECK(VerifyEntitlement(h64 + "." + p64 + "." + Base64UrlEncode(sig), keys, kClientId, T0, VerifyEs256, e) == JwsError::None);
    CHECK(e.IsProAt(T0));
    std::string bad = sig;
    bad[10] ^= 1;
    CHECK(VerifyEntitlement(h64 + "." + p64 + "." + Base64UrlEncode(bad), keys, kClientId, T0, VerifyEs256, e) == JwsError::BadSignature);
    // Payload đổi (nâng pro_until) mà giữ chữ ký cũ => từ chối
    const std::string p2 = Base64UrlEncode(EntPayload(T0, T0 + 86400, R"(,"pro_until":2900000000)"));
    CHECK(VerifyEntitlement(h64 + "." + p2 + "." + Base64UrlEncode(sig), keys, kClientId, T0, VerifyEs256, e) == JwsError::BadSignature);
    // Điểm không nằm trên đường cong => BCryptImportKeyPair từ chối
    Jwk off = jwk;
    off.y[31] ^= 1;
    CHECK(!VerifyEs256(off, h64 + "." + p64, sig));

    BCryptDestroyKey(key);
    BCryptCloseAlgorithmProvider(alg, 0);
}
#endif

int main()
{
    TestBase64Url();
    TestConstantTimeEquals();
    TestJson();
    TestAuthorizationUrl();
    TestCallback();
    TestInstallationId();
    TestJwks();
    TestEntitlement();
    TestIdToken();
    TestResponses();
#ifdef _WIN32
    TestPkceRfc7636();
    TestEs256Real();
#endif
    std::cout << "ProAuthTests: " << g_passed << " dat, " << g_failed << " that bai\n";
    return g_failed ? 1 : 0;
}
