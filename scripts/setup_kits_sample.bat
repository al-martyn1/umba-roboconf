@rem Set up correct paths to perform "Scan For Kits"
@rem Then copy it to setup_kits_sample.bat
@rem copy setup_kits_sample.bat setup_kits.bat
@rem In VSCode press Ctrl+Shift+P and insert "CMake: Scan for Kits"
@rem
@set LLVM_CLANG_BIN=C:\\Program Files\\LLVM\\bin
@set GCC_13_ARM_NONE_EABI_BIN=C:\Program Files (x86)\Arm GNU Toolchain arm-none-eabi\13.2 Rel1\bin
@set GCC_13_W64_DEVKIT_BIN=C:\w64devkit\bin
@set GCC_73_32_QT=C:\Qt\Qt5.12.12\Tools\mingw730_32\bin
@set GCC_73_64_QT=C:\Qt\Qt5.12.12\Tools\mingw730_64\bin
@set GCC_62_MINGW32=C:\MinGW\bin

@set PATH=%LLVM_CLANG_BIN%;%GCC_13_ARM_NONE_EABI_BIN%;%GCC_13_W64_DEVKIT_BIN%;%GCC_73_32_QT%;%GCC_73_64_QT%;%GCC_62_MINGW32%;%PATH%

@rem     "keep": true,
@rem     "isTrusted": true
