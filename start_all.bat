@echo off
echo Starting C Backend...
start "C Backend" backend\server.exe
timeout /t 2 /nobreak >nul

echo Starting React Frontend...
cd frontend
npm run dev
