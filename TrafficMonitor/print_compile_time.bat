REM Ghi thoi diem cua ma nguon (UTC, dinh dang ISO, khong phu thuoc locale) vao compile_time.txt de nhung vao tai nguyen.
REM Build tai lap: uu tien SOURCE_DATE_EPOCH, roi thoi diem commit HEAD (git), chi khi khong co ca hai moi dung gio hien tai.
REM Nho vay hai lan build cung commit cho ra cung TrafficMonitor.exe (xem SECURITY.md muc build tai lap).
REM CCommon::GetLastCompileTime bo CRLF va bo ky tu cuoi (dau cach) cua noi dung tep.
del /F /Q compile_time.txt 2>nul
powershell -NoProfile -NonInteractive -Command "$e = $env:SOURCE_DATE_EPOCH; if (-not $e) { $e = git log -1 --format=%%ct 2>$null }; if ($e -match '^\d+$') { [DateTimeOffset]::FromUnixTimeSeconds([long]$e).UtcDateTime.ToString('yyyy-MM-dd HH:mm:ss') + ' UTC ' } else { (Get-Date).ToUniversalTime().ToString('yyyy-MM-dd HH:mm:ss') + ' UTC ' }" > compile_time.txt
if errorlevel 1 echo unknown > compile_time.txt
exit /b 0
