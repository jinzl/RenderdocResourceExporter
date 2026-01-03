:: 用来调用 RenderdocCSVToFBX.exe 的批处理文件

::cd /d %~dp0
set current_path=%~dp0

echo current_path=%current_path%
%current_path%RenderdocCSVToFBX.exe %*

::pause
