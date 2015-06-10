@echo off

for /d /r . %%d in (ipch) do @if exist "%%d" rd /s /q "%%d"
for /d /r . %%d in (_DEBUG_32_) do @if exist "%%d" rd /s /q "%%d"
for /d /r . %%d in (_DEBUG_64_) do @if exist "%%d" rd /s /q "%%d"
for /d /r . %%d in (_RELEASE_32_) do @if exist "%%d" rd /s /q "%%d"
for /d /r . %%d in (_RELEASE_64_) do @if exist "%%d" rd /s /q "%%d"

del *.log /f /s /q /a

:: MSVC Files
del *.ncb /f /s /q /a
del *.sdf /f /s /q /a
del *.suo /f /s /q /a
del *.user /f /s /q /a
del *.opensdf /f /s /q /a
