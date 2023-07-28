@ECHO OFF

SET "VC2017=%ProgramFiles(x86)%\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
SET "VC2019=%ProgramFiles(x86)%\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
SET "VC2022=%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
SET VCVARS=

:PARSE_ARGS
IF NOT "%1" == "" (
    IF /I "%1" == "VS2017" SET "VCVARS=%VC2017%"
    IF /I "%1" == "VS2019" SET "VCVARS=%VC2019%"
    IF /I "%1" == "VS2022" SET "VCVARS=%VC2022%"
    SHIFT
    GOTO :PARSE_ARGS
)

IF "%VCVARS%" == "" (
    IF EXIST "%VC2022%" (
        SET "VCVARS=%VC2022%"
        GOTO :END
    )
    IF EXIST "%VC2019%" (
        SET "VCVARS=%VC2019%"
        GOTO :END
    )
    IF EXIST "%VC2017%" (
        SET "VCVARS=%VC2017%"
        GOTO :END
    )
)

:END
