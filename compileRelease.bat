@echo off
title SLH-DSA Builder
color 0A

echo Compilando todas las variantes SLH-DSA...
echo.

gradlew.bat clean
if %errorlevel% neq 0 goto :error

echo [1/6] ARM64...
gradlew.bat assembleArm64Release
if %errorlevel% neq 0 goto :error

echo [2/6] ARM32...
gradlew.bat assembleArm32Release
if %errorlevel% neq 0 goto :error

echo [3/6] MOBILE...
gradlew.bat assembleMobileRelease
if %errorlevel% neq 0 goto :error

echo [4/6] X86...
gradlew.bat assembleX86Release
if %errorlevel% neq 0 goto :error

echo [5/6] X64...
gradlew.bat assembleX64Release
if %errorlevel% neq 0 goto :error

echo [6/6] UNIVERSAL...
gradlew.bat assembleUniversalRelease
if %errorlevel% neq 0 goto :error

echo.
echo ✅ Compilacion completada
echo 📦 6 archivos .aar generados en: build\outputs\aar\
explorer build\outputs\aar
goto :end

:error
echo ❌ Error en compilacion
pause
exit /b 1

:end
pause
