cmake_minimum_required(VERSION 4.0)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(project_name VERSION 0.0.${PATCH_VERSION})

set(BOOST_ROOT /opt/homebrew/opt/boost)
find_package(Boost CONFIG REQUIRED)

option(WITH_BOOST_TEST "Whether to build Boost test" ON)


# === Настройка компилятора ===
if(CMAKE_CXX_COMPILER STREQUAL "CMAKE_CXX_COMPILER-NOTFOUND" OR
        CMAKE_CXX_COMPILER MATCHES "apple.*clang")
    find_program(CLANGPP clang++ REQUIRED PATHS /opt/homebrew/bin /usr/bin)
    set(CMAKE_CXX_COMPILER ${CLANGPP})
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # -g: symbols для профайлера
    # -O3: максимальная оптимизация
    # -DNDEBUG: отключает assert
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -O3")
endif()

# === Определяем SDK ТОЛЬКО ПОСЛЕ project() и компилятора ===
if(APPLE)
    # CMake ≥ 3.15+ автоматически вызывает xcrun при установке CMAKE_OSX_SYSROOT=""
    # Но мы сделаем явно — для надёжности
    if(NOT CMAKE_OSX_SYSROOT)
        execute_process(
                COMMAND xcrun --show-sdk-path
                OUTPUT_VARIABLE SDK_PATH
                OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_QUIET
        )
        if(NOT SDK_PATH)
            message(FATAL_ERROR "❌ Не найден macOS SDK. Выполни: xcode-select --install")
        endif()
        set(CMAKE_OSX_SYSROOT "${SDK_PATH}" CACHE STRING "" FORCE)
        message(STATUS "✅ Используется macOS SDK: ${SDK_PATH}")
    endif()

    set(CMAKE_OSX_ARCHITECTURES "arm64")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Let's nicely support folders in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# === Флаги компиляции ===
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Базовые флаги — через add_compile_options (НЕ через CMAKE_CXX_FLAGS!)
    add_compile_options(
            -Wall -Wextra -Wpedantic -Werror
            -Wshadow -Wnon-virtual-dtor -Wold-style-cast
            -Wcast-align -Wunused -Woverloaded-virtual
            -Wnull-dereference -Wdouble-promotion -Wformat=2
            -march=armv8.4-a+fp16+simd+crc+crypto
    )

    if(APPLE)
        # -stdlib=libc++ — ОБЯЗАТЕЛЬНО для Homebrew Clang!
        add_compile_options(-stdlib=libc++)
        add_link_options(-stdlib=libc++)

        # Debug + sanitizers требуют -O1 на macOS
        add_compile_options($<$<CONFIG:Debug>:-O1>)
    endif()

    # Санитайзеры
    option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
    option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

    if(ENABLE_ASAN)
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address)
    endif()
    if(ENABLE_UBSAN)
        add_compile_options(-fsanitize=undefined -fno-omit-frame-pointer)
        add_link_options(-fsanitize=undefined)
    endif()
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

# === Остальное без изменений ===
set(BUILD_TARGET "" CACHE STRING "Target to build (e.g. day1_raii)")
include_directories(libs)
include_directories(include)
include_directories(${Boost_INCLUDE_DIR})

# FetchContent added in CMake 3.11, downloads during the configure step
include(FetchContent)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)

if (WITH_BOOST_TEST)
    enable_testing()
endif ()

if(BUILD_TARGET AND EXISTS "${CMAKE_SOURCE_DIR}/src/${BUILD_TARGET}/CMakeLists.txt")
    add_subdirectory("src/${BUILD_TARGET}")
else()
    file(GLOB TARGET_DIRS RELATIVE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/*)
    foreach(dir ${TARGET_DIRS})
        if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/src/${dir}" AND EXISTS "${CMAKE_SOURCE_DIR}/src/${dir}/CMakeLists.txt")
            add_subdirectory("src/${dir}")
        endif()
    endforeach()
endif()

