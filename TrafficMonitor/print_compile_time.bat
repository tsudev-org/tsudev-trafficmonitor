REM Ghi thoi diem build (UTC, dinh dang ISO, khong phu thuoc locale cua may build) vao compile_time.txt de nhung vao tai nguyen.
REM Truoc day dung %date:~0,10% nen tren may en-US bi cat thanh "Sun 11/08/" (mat nam).
REM CCommon::GetLastCompileTime bo CRLF va bo ky tu cuoi (dau cach) cua noi dung tep.
del /F /Q compile_time.txt 2>nul
powershell -NoProfile -NonInteractive -Command "(Get-Date).ToUniversalTime().ToString('yyyy-MM-dd HH:mm:ss') + ' UTC '" > compile_time.txt
if errorlevel 1 echo unknown > compile_time.txt
exit /b 0
