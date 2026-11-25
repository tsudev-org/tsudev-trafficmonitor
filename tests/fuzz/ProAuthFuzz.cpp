// Fuzz bộ phân tích đầu vào không tin cậy của đăng nhập tsudev (ProAuth.h): JSON (phản hồi token/JWKS/JWS),
// JWS compact, dòng đầu HTTP từ cổng loopback. Build (CI, build.yml):
//   cl /std:c++17 /EHsc /utf-8 /Zi /fsanitize=address /fsanitize=fuzzer /I tsudev-trafficmonitor tests\fuzz\ProAuthFuzz.cpp
//   clang++ -std=c++17 -g -fsanitize=address,undefined,fuzzer -I tsudev-trafficmonitor tests/fuzz/ProAuthFuzz.cpp
#include "ProAuth.h"

static bool AlwaysTrue(const ProAuth::Jwk&, const std::string&, const std::string&) { return true; }

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size > 64 * 1024)
        return 0;
    const std::string in(reinterpret_cast<const char*>(data), size);
    ProAuth::JsonValue v;
    ProAuth::JsonParser::Parse(in, v);
    ProAuth::JwkSet keys;
    ProAuth::ParseJwks(in, keys);
    ProAuth::TokenResponse t;
    ProAuth::ParseTokenResponse(in, t);
    std::string s;
    ProAuth::Base64UrlDecode(in, s);
    ProAuth::ParseCallbackRequestLine(in);
    // Bộ kiểm chữ ký luôn đúng: đẩy fuzzer đi sâu vào đối chiếu claim
    ProAuth::JwkSet one;
    ProAuth::Jwk k;
    k.kid = "k1";
    k.x.assign(32, 'x');
    k.y.assign(32, 'y');
    one.emplace("k1", k);
    ProAuth::Entitlement e;
    ProAuth::VerifyEntitlement(in, one, ProAuth::kClientId, 1'800'000'000, AlwaysTrue, e);
    std::string sub;
    ProAuth::ValidateIdToken(in, one, ProAuth::kClientId, "n", 1'800'000'000, AlwaysTrue, sub);
    return 0;
}
