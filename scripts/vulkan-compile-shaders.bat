@ECHO OFF

SET GLSLPath=%~1
SET AssetsPath=%2

PUSHD %AssetsPath%
ECHO Compiling vertex shaders in %AssetsPath%
FOR /R %%g in (*.vert.glsl) do (CALL :compile %%g "vert")

ECHO Compiling fragment shaders in %AssetsPath%
FOR /R %%g in (*.frag.glsl) do (CALL :compile %%g "frag")
POPD

EXIT /B

:compile
SETLOCAL

SET FilePath=%~f1
SET FileName=%~n1
SET FileExtension=%~x1
SET OutputFile=%FilePath:.glsl=%.spv
SET Type=%~2
SET Command="%GLSLPath%\glslc.exe" -fshader-stage=%Type% %FilePath% -o %OutputFile%

ECHO %FileName% to %OutputFile%

::ECHO %Command%
%Command%

if %ERRORLEVEL% NEQ 0 (echo Error compiling %FileName%: %ERRORLEVEL% && exit)

ENDLOCAL
EXIT /B