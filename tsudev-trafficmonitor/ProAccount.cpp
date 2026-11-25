// Tài khoản tsudev (P9 phần 3) - xem ProAccount.h. Tệp này KHÔNG dùng stdafx.h (PrecompiledHeader = NotUsing):
// ProNet.h phải kéo winsock2.h trước windows.h, còn stdafx.h của MFC đã kéo windows.h (kèm winsock.h cũ).

#include "ProNet.h"
#include "ProAccount.h"
#include <shellapi.h>
#include <objbase.h>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <fstream>
#include <mutex>
#include <thread>
#include <ctime>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")

namespace
{
    using Clock = std::chrono::steady_clock;
    constexpr size_t kMaxCacheFile = 64 * 1024;

    bool ReadSmallFile(const std::wstring& path, std::string& out)
    {
        out.clear();
        std::ifstream f(path.c_str(), std::ios::binary);
        if (!f)
            return false;
        char buf[4096];
        while (f.read(buf, sizeof(buf)) || f.gcount() > 0)
        {
            out.append(buf, static_cast<size_t>(f.gcount()));
            if (out.size() > kMaxCacheFile)
            {
                out.clear();
                return false;
            }
        }
        return !out.empty();
    }

    // Ghi tệp tạm rồi thay thế (không để lại tệp ghi dở khi máy tắt giữa chừng)
    bool WriteFileReplace(const std::wstring& path, const std::string& data)
    {
        const std::wstring tmp = path + L".tmp";
        {
            std::ofstream f(tmp.c_str(), std::ios::binary | std::ios::trunc);
            if (!f)
                return false;
            f.write(data.data(), static_cast<std::streamsize>(data.size()));
            if (!f)
                return false;
        }
        if (!MoveFileExW(tmp.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
        {
            DeleteFileW(tmp.c_str());
            return false;
        }
        return true;
    }

    void WipeString(std::string& s)
    {
        if (!s.empty())
            SecureZeroMemory(&s[0], s.size());
        s.clear();
    }

    std::wstring AsciiWiden(const std::string& s)
    {
        return std::wstring(s.begin(), s.end());
    }
}

ProAccountDeps ProAccountDeps::Default()
{
    ProAccountDeps d;
    d.http = [](const wchar_t* method, const std::string& url, const std::wstring& headers, const std::string& body) {
        ProNet::HttpResult r = ProNet::HttpsRequest(method, url, headers, body);
        ProHttpResult out;
        out.ok = r.ok;
        out.status = r.status;
        out.body = std::move(r.body);
        return out;
    };
    d.open_browser = [](const std::string& url) {
        // Chỉ mở URL đăng nhập của tsudev.com (phòng thủ nhiều lớp: URL do ProAuth dựng, không lấy từ ngoài)
        if (url.compare(0, std::strlen(ProAuth::kAuthorizationEndpoint), ProAuth::kAuthorizationEndpoint) != 0)
            return false;
        const std::wstring w = ProNet::Widen(url);
        return reinterpret_cast<INT_PTR>(ShellExecuteW(nullptr, L"open", w.c_str(), nullptr, nullptr, SW_SHOWNORMAL)) > 32;
    };
    d.verify = ProAuth::VerifyEs256;
    d.save_secret = [](const std::string& s) { return ProNet::SaveSecret(s); };
    d.load_secret = [](std::string& s) { return ProNet::LoadSecret(s); };
    d.delete_secret = [] { return ProNet::DeleteSecret(); };
    d.now = [] { return static_cast<int64_t>(std::time(nullptr)); };
    return d;
}

struct CProAccount::Impl
{
    ProAccountDeps deps;
    mutable std::mutex mu;
    std::condition_variable cv;        // hàng lệnh / dừng
    std::condition_variable cv_done;   // luồng nền đã thoát
    std::deque<Op> queue;
    bool stop = false;
    bool finished = true;
    volatile bool cancel_login = false;
    std::atomic<bool> offline{ false };
    uint64_t epoch = 0;                // tăng khi đăng xuất: kết quả của lượt cũ bị bỏ
    std::string installation_id;
    std::string pending_revoke;        // refresh token chờ thu hồi phía máy chủ sau khi đã xoá cục bộ
    std::wstring dir;
    HWND notify_wnd = nullptr;
    UINT notify_msg = 0;
    Status st;
    Clock::time_point next_refresh = Clock::now() + std::chrono::hours(24 * 365);

    explicit Impl(ProAccountDeps d) : deps(std::move(d)) {}

    std::wstring EntPath() const { return dir + CProAccount::kEntitlementFile; }
    std::wstring JwksPath() const { return dir + CProAccount::kJwksFile; }

    void Notify()
    {
        HWND w = nullptr;
        UINT m = 0;
        {
            std::lock_guard<std::mutex> lk(mu);
            w = notify_wnd;
            m = notify_msg;
        }
        if (w && m)
            PostMessageW(w, m, 0, 0);
    }

    template <class F>
    void Update(F f)
    {
        {
            std::lock_guard<std::mutex> lk(mu);
            f(st);
            st.revision++;
        }
        Notify();
    }

    // Kết quả chỉ áp dụng nếu chưa có lệnh đăng xuất chen vào
    template <class F>
    bool UpdateIfEpoch(uint64_t e, F f)
    {
        bool applied = false;
        {
            std::lock_guard<std::mutex> lk(mu);
            if (epoch == e)
            {
                f(st);
                st.revision++;
                applied = true;
            }
        }
        if (applied)
            Notify();
        return applied;
    }

    void SetError(uint64_t e, Error err)
    {
        UpdateIfEpoch(e, [err](Status& s) { s.last_error = err; });
    }

    void ClearCacheFiles()
    {
        DeleteFileW(EntPath().c_str());
        DeleteFileW(JwksPath().c_str());
    }

    // Nạp JWS + JWKS đệm và KIỂM LẠI chữ ký (tệp sửa tay không thành Pro)
    void LoadCache()
    {
        std::string jws, jwks;
        ProAuth::JwkSet keys;
        ProAuth::Entitlement ent;
        bool ok = ReadSmallFile(EntPath(), jws) && ReadSmallFile(JwksPath(), jwks) && ProAuth::ParseJwks(jwks, keys)
            && ProAuth::VerifyEntitlement(jws, keys, ProAuth::kClientId, deps.now(), deps.verify, ent) == ProAuth::JwsError::None;
        std::lock_guard<std::mutex> lk(mu);
        st.has_entitlement = ok;
        st.entitlement = ok ? ent : ProAuth::Entitlement();
        st.revision++;
    }

    bool HttpAllowed() const { return !offline.load(); }

    // GET JWKS (khoá công khai ký id_token và JWS quyền lợi)
    Error FetchJwks(ProAuth::JwkSet& keys, std::string& json)
    {
        if (!HttpAllowed()) return Error::Offline;
        ProHttpResult r = deps.http(L"GET", ProAuth::kJwksUrl, L"", "");
        if (!r.ok) return Error::Network;
        if (r.status != 200 || !ProAuth::ParseJwks(r.body, keys) || keys.empty()) return Error::Server;
        json = std::move(r.body);
        return Error::None;
    }

    Error FetchEntitlement(uint64_t e, const std::string& access_token, const ProAuth::JwkSet& keys, const std::string& jwks_json,
                           const std::string& expected_sub)
    {
        if (!HttpAllowed()) return Error::Offline;
        std::wstring headers = L"Authorization: Bearer " + AsciiWiden(access_token) + L"\r\n";
        std::string id;
        {
            std::lock_guard<std::mutex> lk(mu);
            id = installation_id;
        }
        if (ProAuth::IsValidInstallationId(id))     // AUTH 14.3: chỉ gửi khi đã đăng nhập (đang ở đây)
            headers += AsciiWiden(std::string(ProAuth::kInstallationHeader) + ": " + id + "\r\n");
        ProHttpResult r = deps.http(L"GET", ProAuth::kEntitlementsUrl, headers, "");
        if (!r.ok) return Error::Network;
        if (r.status == 401) return Error::SessionExpired;
        std::string jws;
        if (r.status != 200 || !ProAuth::ParseEntitlementResponse(r.body, jws)) return Error::Server;
        ProAuth::Entitlement ent;
        const int64_t now = deps.now();
        if (ProAuth::VerifyEntitlement(jws, keys, ProAuth::kClientId, now, deps.verify, ent) != ProAuth::JwsError::None)
            return Error::InvalidToken;
        if (!expected_sub.empty() && ent.sub != expected_sub)
            return Error::InvalidToken;
        UpdateIfEpoch(e, [&](Status& s) {
            WriteFileReplace(EntPath(), jws);
            WriteFileReplace(JwksPath(), jwks_json);
            s.has_entitlement = true;
            s.entitlement = ent;
            s.last_online_ok = now;
            s.last_error = Error::None;
        });
        return Error::None;
    }

    void DoSignIn(uint64_t e)
    {
        if (!HttpAllowed()) { SetError(e, Error::Offline); return; }
        ProNet::LoopbackReceiver rx;
        if (!rx.IsListening()) { SetError(e, Error::Listener); return; }
        ProAuth::AuthorizationRequest req = ProAuth::NewAuthorizationRequest(rx.Port());
        if (req.state.empty()) { SetError(e, Error::Listener); return; }
        if (!deps.open_browser(req.Url())) { SetError(e, Error::Browser); return; }
        ProAuth::CallbackResult cb = rx.Wait(deps.login_timeout_ms, &cancel_login);
        if (!cb.ok) { SetError(e, Error::Cancelled); return; }
        // Thứ tự kiểm: state trước (chặn tiến trình khác trên máy gọi cổng loopback), rồi iss (RFC 9207)
        if (!req.StateMatches(cb.state) || !cb.IssuerOk()) { SetError(e, Error::BadCallback); return; }
        if (!cb.error.empty()) { SetError(e, cb.error == "access_denied" ? Error::Denied : Error::Server); return; }
        if (!HttpAllowed()) { SetError(e, Error::Offline); return; }

        ProHttpResult r = deps.http(L"POST", ProAuth::kTokenEndpoint, L"", req.TokenRequestBody(cb.code));
        if (!r.ok) { SetError(e, Error::Network); return; }
        ProAuth::TokenResponse tr;
        if (r.status != 200 || !ProAuth::ParseTokenResponse(r.body, tr) || tr.id_token.empty() || tr.refresh_token.empty())
        {
            SetError(e, Error::Server);
            return;
        }
        ProAuth::JwkSet keys;
        std::string jwks_json;
        Error err = FetchJwks(keys, jwks_json);
        if (err != Error::None) { SetError(e, err); return; }
        std::string sub;
        if (ProAuth::ValidateIdToken(tr.id_token, keys, ProAuth::kClientId, req.nonce, deps.now(), deps.verify, sub) != ProAuth::JwsError::None)
        {
            SetError(e, Error::InvalidToken);
            return;
        }
        {
            std::lock_guard<std::mutex> lk(mu);
            if (epoch != e) return;       // đã đăng xuất trong lúc chờ: không lưu gì
        }
        if (!deps.save_secret(tr.refresh_token)) { SetError(e, Error::Storage); WipeString(tr.refresh_token); return; }
        WipeString(tr.refresh_token);
        UpdateIfEpoch(e, [](Status& s) { s.signed_in = true; s.last_error = Error::None; });
        err = FetchEntitlement(e, tr.access_token, keys, jwks_json, sub);
        WipeString(tr.access_token);
        if (err != Error::None) SetError(e, err);
        ScheduleRefresh(err == Error::None ? deps.refresh_interval_ms : deps.retry_interval_ms);
    }

    void ExpireSession(uint64_t e)
    {
        bool applied = UpdateIfEpoch(e, [](Status& s) {
            s.signed_in = false;
            s.has_entitlement = false;
            s.entitlement = ProAuth::Entitlement();
            s.last_error = Error::SessionExpired;
        });
        if (applied)
        {
            deps.delete_secret();
            ClearCacheFiles();
        }
    }

    void DoRefresh(uint64_t e, bool manual)
    {
        if (!HttpAllowed()) { if (manual) SetError(e, Error::Offline); return; }
        std::string rt;
        if (!deps.load_secret(rt))
        {
            UpdateIfEpoch(e, [](Status& s) { s.signed_in = false; });
            return;
        }
        ProHttpResult r = deps.http(L"POST", ProAuth::kTokenEndpoint, L"", ProAuth::RefreshRequestBody(rt));
        if (!r.ok) { WipeString(rt); SetError(e, Error::Network); ScheduleRefresh(deps.retry_interval_ms); return; }
        ProAuth::TokenResponse tr;
        bool parsed = ProAuth::ParseTokenResponse(r.body, tr);
        if (!parsed && (r.status == 400 || r.status == 401) && tr.error == "invalid_grant")
        {
            WipeString(rt);
            ExpireSession(e);     // bị thu hồi / hết hạn / đã đăng xuất ở nơi khác => chế độ khách
            return;
        }
        if (r.status != 200 || !parsed) { WipeString(rt); SetError(e, Error::Server); ScheduleRefresh(deps.retry_interval_ms); return; }
        // Xoay vòng refresh token: lưu token mới (nếu chưa đăng xuất chen vào)
        if (!tr.refresh_token.empty() && tr.refresh_token != rt)
        {
            bool same_epoch;
            {
                std::lock_guard<std::mutex> lk(mu);
                same_epoch = epoch == e;
            }
            if (same_epoch && !deps.save_secret(tr.refresh_token))
                SetError(e, Error::Storage);
        }
        WipeString(rt);
        WipeString(tr.refresh_token);
        ProAuth::JwkSet keys;
        std::string jwks_json;
        Error err = FetchJwks(keys, jwks_json);
        if (err == Error::None)
            err = FetchEntitlement(e, tr.access_token, keys, jwks_json, std::string());
        WipeString(tr.access_token);
        if (err == Error::SessionExpired) { ExpireSession(e); return; }
        if (err != Error::None) SetError(e, err);
        ScheduleRefresh(err == Error::None ? deps.refresh_interval_ms : deps.retry_interval_ms);
    }

    // Thu hồi phía máy chủ (RFC 7009), cố gắng một lần; dữ liệu cục bộ đã xoá ở SignOutAndClear
    void DoRevoke()
    {
        std::string rt;
        {
            std::lock_guard<std::mutex> lk(mu);
            rt.swap(pending_revoke);
        }
        if (!rt.empty() && HttpAllowed())
        {
            deps.http(L"POST", ProAuth::kRevocationEndpoint, L"",
                "token=" + ProAuth::UrlEncode(rt) + "&token_type_hint=refresh_token&client_id=" + ProAuth::UrlEncode(ProAuth::kClientId));
        }
        WipeString(rt);
    }

    void ScheduleRefresh(int ms)
    {
        std::lock_guard<std::mutex> lk(mu);
        next_refresh = Clock::now() + std::chrono::milliseconds(ms);
    }

    void Run()
    {
        const HRESULT co = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);   // ShellExecute
        for (;;)
        {
            Op op = Op::None;
            bool manual = true;
            uint64_t e = 0;
            {
                std::unique_lock<std::mutex> lk(mu);
                cv.wait_until(lk, next_refresh, [this] { return stop || !queue.empty() || Clock::now() >= next_refresh; });
                if (stop)
                    break;
                if (!queue.empty())
                {
                    op = queue.front();
                    queue.pop_front();
                }
                else
                {
                    next_refresh = Clock::now() + std::chrono::milliseconds(deps.refresh_interval_ms);
                    if (!st.signed_in || offline.load())
                        continue;
                    op = Op::Refreshing;
                    manual = false;
                }
                if (op == Op::SigningIn)
                    cancel_login = false;
                e = epoch;
                st.op = op;
                st.revision++;
            }
            Notify();
            switch (op)
            {
            case Op::SigningIn: DoSignIn(e); break;
            case Op::Refreshing: DoRefresh(e, manual); break;
            case Op::SigningOut: DoRevoke(); break;
            default: break;
            }
            Update([](Status& s) { s.op = Op::None; });
        }
        if (SUCCEEDED(co))
            CoUninitialize();
        std::lock_guard<std::mutex> lk(mu);
        finished = true;
        cv_done.notify_all();
    }

    void Push(Op op)
    {
        {
            std::lock_guard<std::mutex> lk(mu);
            // Không xếp trùng lệnh đang chờ (bấm nhiều lần)
            for (Op q : queue)
                if (q == op) return;
            queue.push_back(op);
        }
        cv.notify_all();
    }
};

CProAccount::CProAccount(ProAccountDeps deps) : m_impl(std::make_shared<Impl>(std::move(deps))) {}

CProAccount::~CProAccount()
{
    Shutdown();
}

void CProAccount::Init(const std::wstring& config_dir, const std::string& installation_id, bool offline, void* notify_wnd, unsigned notify_msg)
{
    Impl& d = *m_impl;
    {
        std::lock_guard<std::mutex> lk(d.mu);
        if (!d.finished)
            return;     // đã chạy
        d.dir = config_dir;
        d.installation_id = installation_id;
        d.notify_wnd = static_cast<HWND>(notify_wnd);
        d.notify_msg = notify_msg;
        d.stop = false;
        d.finished = false;
    }
    d.offline = offline;
    std::string secret;
    const bool signed_in = d.deps.load_secret && d.deps.load_secret(secret);   // đọc cục bộ, không gọi mạng
    WipeString(secret);
    d.LoadCache();
    {
        std::lock_guard<std::mutex> lk(d.mu);
        d.st.signed_in = signed_in;
        if (signed_in)
            d.next_refresh = Clock::now() + std::chrono::milliseconds(d.deps.first_refresh_delay_ms);
    }
    std::shared_ptr<Impl> keep = m_impl;
    std::thread([keep] { keep->Run(); }).detach();
}

void CProAccount::SetOffline(bool offline)
{
    m_impl->offline = offline;
    if (offline)
        CancelSignIn();
    m_impl->Update([](Status&) {});
}

void CProAccount::SetInstallationId(const std::string& installation_id)
{
    std::lock_guard<std::mutex> lk(m_impl->mu);
    m_impl->installation_id = installation_id;
}

void CProAccount::StartSignIn()
{
    if (m_impl->offline.load())
    {
        m_impl->Update([](Status& s) { s.last_error = Error::Offline; });
        return;
    }
    m_impl->Push(Op::SigningIn);
}

void CProAccount::CancelSignIn()
{
    std::lock_guard<std::mutex> lk(m_impl->mu);
    m_impl->cancel_login = true;
    for (auto it = m_impl->queue.begin(); it != m_impl->queue.end();)
        it = (*it == Op::SigningIn) ? m_impl->queue.erase(it) : it + 1;
}

void CProAccount::RefreshNow()
{
    m_impl->Push(Op::Refreshing);
}

void CProAccount::SignOutAndClear()
{
    Impl& d = *m_impl;
    std::string rt;
    if (d.deps.load_secret)
        d.deps.load_secret(rt);
    if (d.deps.delete_secret)
        d.deps.delete_secret();
    d.ClearCacheFiles();
    {
        std::lock_guard<std::mutex> lk(d.mu);
        d.epoch++;
        d.cancel_login = true;
        d.queue.clear();
        d.st.signed_in = false;
        d.st.has_entitlement = false;
        d.st.entitlement = ProAuth::Entitlement();
        d.st.last_online_ok = 0;
        d.st.last_error = Error::None;
        d.st.revision++;
        WipeString(d.pending_revoke);
        d.pending_revoke = std::move(rt);
        d.next_refresh = Clock::now() + std::chrono::hours(24 * 365);
    }
    d.Notify();
    if (!d.pending_revoke.empty())
        d.Push(Op::SigningOut);
}

void CProAccount::Shutdown(int wait_ms)
{
    Impl& d = *m_impl;
    std::unique_lock<std::mutex> lk(d.mu);
    if (d.finished)
        return;
    d.stop = true;
    d.cancel_login = true;
    d.notify_wnd = nullptr;
    d.cv.notify_all();
    // Luồng đã tách: chờ có giới hạn; quá hạn (đang giữa một lượt HTTPS) thì để nó tự kết thúc cùng tiến trình
    d.cv_done.wait_for(lk, std::chrono::milliseconds(wait_ms), [&d] { return d.finished; });
}

CProAccount::Status CProAccount::GetStatus() const
{
    std::lock_guard<std::mutex> lk(m_impl->mu);
    return m_impl->st;
}

bool CProAccount::RenewalInput(int64_t& exp, int64_t& pro_until) const
{
    std::lock_guard<std::mutex> lk(m_impl->mu);
    if (!m_impl->st.has_entitlement)
        return false;
    exp = m_impl->st.entitlement.exp;
    pro_until = m_impl->st.entitlement.pro_until;
    return true;
}
