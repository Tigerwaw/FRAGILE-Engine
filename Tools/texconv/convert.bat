@echo off
setlocal

title PNG to DDS Converter

set executing_directory=%~dp0

set content_root=%executing_directory%..\..\Assets\
set output_path=%executing_directory%..\..\Assets\

set common_args=-nologo -y -ft dds -o %output_path% -r:keep %content_root%

start /b %executing_directory%texconv %common_args%*_C.png -f BC7_UNORM_SRGB -srgbi
start /b %executing_directory%texconv %common_args%*_M.png -f BC7_UNORM
start /b %executing_directory%texconv %common_args%*_FX.png -f BC7_UNORM
start /b %executing_directory%texconv %common_args%*_N.png -f BC5_UNORM

start /b %executing_directory%texconv %common_args%*_F.png -f BC7_UNORM

exit