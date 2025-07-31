@echo off
setlocal

title Compile Shaders

set executing_directory=%~dp0..\..\Assets\

for /r %executing_directory% %%f in (*_VS.hlsl*) do (
fxc /T vs_5_0 /Fo %%~df%%~pf\%%~nf.cso %%f
)

for /r %executing_directory% %%f in (*_GS.hlsl*) do (
fxc /T gs_5_0 /Fo %%~df%%~pf\%%~nf.cso %%f
)

for /r %executing_directory% %%f in (*_PS.hlsl*) do (
fxc /T ps_5_0 /Fo %%~df%%~pf\%%~nf.cso %%f
)

for /r %executing_directory% %%f in (*_CS.hlsl*) do (
fxc /T ps_5_0 /Fo %%~df%%~pf\%%~nf.cso %%f
)

pause