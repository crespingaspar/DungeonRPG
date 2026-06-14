@echo off
chcp 65001 >nul
cd D:\gameproject
echo.
echo === Git Auto Push ===
set /p msg="请输入提交信息: "
git add .
git commit -m "%msg%"
git push origin main
echo.
echo === 推送完成 ===
pause