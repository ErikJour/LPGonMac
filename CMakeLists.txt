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
        "-framework CoreMedia")

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

# We compile Metal ourselves above, but still want the .metal
# file visible/editable inside Xcode.
set_source_files_properties(
        Shaders/shaders.metal
        PROPERTIES
        HEADER_FILE_ONLY TRUE
)

