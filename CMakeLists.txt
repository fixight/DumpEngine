cmake_minimum_required (VERSION 3.10)

# --- БЛОК ПОДКЛЮЧЕНИЯ VCPKG (ВСТАВИТЬ В САМОЕ НАЧАЛО!) ---
if(MSVC)
    add_compile_options(/utf-8)
endif()
# 1. Указываем путь к скрипту Vcpkg (который мы скачали на диск C)
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "" FORCE)

# 2. Указываем архитектуру (Мы качали x64-windows, значит и просим x64)
# Без этой строчки VS может искать x86 и не находить.
set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "" FORCE)

# ----------------------------------------------------------

# Включаем стандарт C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project ("DumpEngine")

# Подключаем папку с кодом
add_subdirectory ("DumpEngine")