@echo off

echo.
echo Copying release build to output directory
echo.

mkdir out

set TargetFile=ImageOptimizer.exe
echo|set /p=copy %TargetFile%...
xcopy Release\%TargetFile%  out\    /FYR  >nul 2>nul
IF %errorlevel%==0 (
     echo OK!
) ELSE (
     echo ERROR!!!
)

set TargetFile=opencv_core310.dll
echo|set /p=copy %TargetFile%...
xcopy Release\%TargetFile%  out\    /FYR  >nul 2>nul
IF %errorlevel%==0 (
     echo OK!
) ELSE (
     echo ERROR!!!
)

set TargetFile=opencv_imgcodecs310.dll
echo|set /p=copy %TargetFile%...
xcopy Release\%TargetFile%  out\    /FYR  >nul 2>nul
IF %errorlevel%==0 (
     echo OK!
) ELSE (
     echo ERROR!!!
)

set TargetFile=opencv_imgproc310.dll
echo|set /p=copy %TargetFile%...
xcopy Release\%TargetFile%  out\    /FYR  >nul 2>nul
IF %errorlevel%==0 (
     echo OK!
) ELSE (
     echo ERROR!!!
)
