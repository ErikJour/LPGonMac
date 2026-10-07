cmake_minimum_required(VERSION 4.1)

project(HomemadeLPG
        LANGUAGES
        C
        CXX
        OBJC
        OBJCXX
)

set(CMAKE_C_STANDARD 11)

add_executable(HomemadeLPG MACOSX_BUNDLE
        main.mm
        mainWindow.h
        mtkViewDelegate.h
        RenderBuffers.h
        GameRenderer.h
        Geometries/SquareGeo.h
        Geometries/TriangleGeo.h
        Audio/AudioUtilities.h
        Audio/AudioRenderer.h
        Audio/DSP/TestToneGenerator.h
        Audio/AudioTypes.h
)

#======================================================
#Automatically manage memory
#======================================================
target_compile_options(HomemadeLPG PRIVATE
        -fobjc-arc)
#======================================================
#Plist for metadata settings
#======================================================
set_target_properties(HomemadeLPG PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST
        ${CMAKE_CURRENT_SOURCE_DIR}/info.plist)

target_link_libraries(HomemadeLPG
        PRIVATE
        "-framework AppKit"
        "-framework Metal"
        "-framework MetalKit"
        "-framework QuartzCore"
        "-framework AVFoundation"
        "-framework CoreVideo"
        "-framework CoreMedia"
        "-framework AudioToolbox"
)

#=========================================================
#Metal
#=========================================================
set(METAL_AIR
        ${CMAKE_CURRENT_BINARY_DIR}/shaders.air
)

set(METALLIB
        ${CMAKE_CURRENT_BINARY_DIR}/shaders.metallib
)

add_custom_command(
        OUTPUT ${METALLIB}

        COMMAND xcrun -sdk macosx metal
        -c
        ${CMAKE_CURRENT_SOURCE_DIR}/Shaders/shaders.metal
        -o ${METAL_AIR}

        COMMAND xcrun -sdk macosx metallib
        ${METAL_AIR}
        -o ${METALLIB}

        DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/Shaders/shaders.metal

        COMMENT "Compiling Metal shaders"
)

target_sources(HomemadeLPG PRIVATE
        ${METALLIB}
)

set_source_files_properties(
        ${METALLIB}
        PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources
        GENERATED TRUE
)

set_source_files_properties(
        Shaders/shaders.metal
        PROPERTIES
        HEADER_FILE_ONLY TRUE
)

option(ENABLE_ASAN "Build with AddressSanitizer" OFF)
option(ENABLE_TSAN "Build with ThreadSanitizer" OFF)

if (ENABLE_ASAN AND ENABLE_TSAN)
    message(FATAL_ERROR "ASan and TSan can't be enabled together")
endif()

if (ENABLE_ASAN)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer -g)
    add_link_options(-fsanitize=address)
elseif (ENABLE_TSAN)
    add_compile_options(-fsanitize=thread -g)
    add_link_options(-fsanitize=thread)
endif()
