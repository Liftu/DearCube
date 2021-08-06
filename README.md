# DearCube

## Compile & Install

### MS Detours 

First download Detours from https://github.com/microsoft/detours </br>
Then, from the Developper command prompt inside the Detours downloaded folder :
```
SET DETOURS_TARGET_PROCESSOR=X86
“%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat” x86
NMAKE
```
Then, copy the lib, sources and headers file to the project directory.

### DearCube

```
git clone --recursive -j8 https://github.com/Liftu/DearCube.git
```
