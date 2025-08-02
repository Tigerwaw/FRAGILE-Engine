@echo off
set executing_directory=%~dp0
set content_root=%executing_directory%..\..\Assets\
cd %content_root%
echo:
for /R %%x in (*.ttf) do (
	echo "%%~nx"
   	%executing_directory%\msdf-atlas-gen.exe -font "%%x" -imageout "%%~dpnx_F.png" -json "%%~dpnx.FONT" -type mtsdf -format png -dimensions 2048 2048 -emrange 0.05 -charset %executing_directory%\charset.txt
	echo:
)

pause