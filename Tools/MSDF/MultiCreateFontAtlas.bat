@echo off
echo:
for /R %%x in (*.ttf) do (
	echo "%%~nx"
   	msdf-atlas-gen.exe -font "%%x" -imageout "T_%%~nx.png" -json "F_%%~nx.json" -type mtsdf -format png -dimensions 2048 2048 -emrange 0.05 -charset charset.txt
	echo:
)

pause