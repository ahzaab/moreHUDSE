cmake_minimum_required(VERSION 3.21)
message("Using toolchain file ${CMAKE_TOOLCHAIN_FILE}.")

########################################################################################################################
## Define project
########################################################################################################################
project(
		AHZmoreHUDPlugin
		VERSION 5.4.3.0
        LANGUAGES CXX
        DESCRIPTION "SKSE64 moreHUD plugin")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Preserve full optimization while emitting linker symbols for crash-log resolution.
if(MSVC)
    add_compile_options("$<$<CONFIG:Release>:/Zi>")
    add_link_options("$<$<CONFIG:Release>:/DEBUG:FULL;/OPT:REF;/OPT:ICF>")
endif()

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

# Build one DLL for Skyrim SE, AE, and VR using the pinned CommonLibSSE-NG
# submodule. Keep CommonLib's own test suite out of this consumer build.
set(ENABLE_SKYRIM_SE ON CACHE BOOL "Enable Skyrim SE support" FORCE)
set(ENABLE_SKYRIM_AE ON CACHE BOOL "Enable Skyrim AE support" FORCE)
set(ENABLE_SKYRIM_VR ON CACHE BOOL "Enable Skyrim VR support" FORCE)
set(SKSE_SUPPORT_XBYAK ON CACHE BOOL "Enable Xbyak trampoline support" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "Build dependency tests" FORCE)
add_subdirectory(external/CommonLibSSE-NG)
include(external/CommonLibSSE-NG/cmake/CommonLibSSE.cmake)

# Define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING to suppress the specific deprecation warning
add_compile_definitions(_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)

# Alternatively, to suppress all MS extension deprecation warnings:
add_compile_definitions(_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS)

include(GNUInstallDirs)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/Version.h.in
	${CMAKE_CURRENT_BINARY_DIR}/include/Version.h
	@ONLY
)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in
	${CMAKE_CURRENT_BINARY_DIR}/version.rc
	@ONLY
)

set(headers
    include/PCH.h
    include/ActorValueList.h
    include/AHZActorInfo.h
    include/AHZArmorInfo.h
    include/AHZConsole.h
    include/AHZExternalFormTable.h 
    include/AHZForEachScriptObjectFunctor.h
    include/AHZFormLookup.h        
    include/AHZLUTObject.h
    include/AHZPapyrusMoreHud.h    
    include/AHZPlayerInfo.h        
    include/AHZScaleform.h
    include/AHZScaleformHook.h     
    include/AHZUtilities.h
    include/AHZVanillaFormTable.h  
    include/AHZWeaponInfo.h        
    include/Events.h
    include/CompletionistAPI.h
    include/HashUtil.h
    include/IForEachScriptObjectFunctor.h
    include/Papyrus.h
    include/Patches.h
    include/Scaleform.h
    include/version.h
    include/AHZTarget.h
)

set(sources
    src/PCH.cpp
    src/ActorValueList.cpp
    src/AHZActorInfo.cpp
    src/AHZArmorInfo.cpp
    src/AHZConsole.cpp
    src/AHZExternalFormTable.cpp   
    src/AHZForEachScriptObjectFunctor.cpp
    src/AHZFormLookup.cpp
    src/AHZLUTObject.cpp
    src/AHZPapyrusMoreHud.cpp      
    src/AHZPlayerInfo.cpp
    src/AHZScaleform.cpp
    src/AHZScaleformHook.cpp       
    src/AHZUtilities.cpp
    src/AHZVanillaFormTable.cpp    
    src/AHZWeaponInfo.cpp
    src/Events.cpp
    src/HashUtil.cpp
    src/IForEachScriptObjectFunctor.cpp
    src/main.cpp
    src/Papyrus.cpp
    src/Patches.cpp
    src/Scaleform.cpp
    src/AHZTarget.cpp

    ${CMAKE_CURRENT_BINARY_DIR}/version.rc
)

source_group(
        TREE ${CMAKE_CURRENT_SOURCE_DIR}
        FILES
        ${headers}
        ${sources})

source_group(
	TREE ${CMAKE_CURRENT_BINARY_DIR}
	FILES
	${CMAKE_CURRENT_BINARY_DIR}/include/Version.h
)

#########################################################################################################################
### Build options
#########################################################################################################################


########################################################################################################################
## Configure target DLL
########################################################################################################################
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(ryml CONFIG REQUIRED)
#find_package(spdlog REQUIRED)
find_package(xbyak REQUIRED)
find_path(ARTICUNO_INCLUDE_DIRS "articuno/articuno.h")

add_commonlibsse_plugin(${PROJECT_NAME}
        NAME "Ahzaab's moreHUD Plugin"
        AUTHOR "Ahzaab"
        VERSION ${PROJECT_VERSION}
        USE_ADDRESS_LIBRARY
        SOURCES ${headers} ${sources})
add_library("${PROJECT_NAME}::${PROJECT_NAME}" ALIAS "${PROJECT_NAME}")

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/include
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
        $<INSTALL_INTERFACE:src>
        ${ARTICUNO_INCLUDE_DIRS})

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        fmt::fmt
        ryml::ryml
		spdlog::spdlog
		xbyak::xbyak
        Version.lib)

target_precompile_headers(${PROJECT_NAME}
        PRIVATE
        include/PCH.h)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(TARGETS ${PROJECT_NAME}
        DESTINATION "${CMAKE_INSTALL_LIBDIR}")





########################################################################################################################
## Configure unit tests
########################################################################################################################


########################################################################################################################
## Automatic plugin deployment
########################################################################################################################
# Automatic deployment to FOMOD directory.
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_NAME "Debug")
else()
    set(BUILD_NAME "Release")
endif()
install(DIRECTORY DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/contrib/Distribution/Papyrus${BUILD_NAME}/")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/contrib/Distribution/Plugin${BUILD_NAME}/")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> "${CMAKE_CURRENT_SOURCE_DIR}/contrib/Distribution/Plugin${BUILD_NAME}/")
if(CMAKE_BUILD_TYPE MATCHES "^(Debug|RelWithDebInfo|Release)$")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PDB_FILE:${PROJECT_NAME}> "${CMAKE_CURRENT_SOURCE_DIR}/contrib/Distribution/Plugin${BUILD_NAME}/")
endif()
file(GLOB_RECURSE OUTPUT_DLLS "${CMAKE_CURRENT_SOURCE_DIR}/contrib/Distribution/**/*.dll")
file(GLOB_RECURSE OUTPUT_PDBS "${CMAKE_CURRENT_SOURCE_DIR}/contrib/Distribution/**/*.pdb")
file(GLOB_RECURSE OUTPUT_SCRIPTS "${CMAKE_CURRENT_SOURCE_DIR}/contrib/Distribution/**/*.pex")
set_property(TARGET ${PROJECT_NAME}
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${OUTPUT_DLLS}")
set_property(TARGET ${PROJECT_NAME}
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${OUTPUT_PDBS}")
set_property(TARGET ${PROJECT_NAME}
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${OUTPUT_SCRIPTS}")

# Automatic deployment to Mod Organizer 2 mod directory.
foreach (DEPLOY_TARGET $ENV{SkyrimPluginTargets})
    set(PLUGIN_DEPLOY_DIRECTORY "${DEPLOY_TARGET}/SKSE/Plugins")
    message("Adding deployment target ${PLUGIN_DEPLOY_DIRECTORY}.")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${PLUGIN_DEPLOY_DIRECTORY}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_FILE:${PROJECT_NAME}> "${PLUGIN_DEPLOY_DIRECTORY}/$<TARGET_FILE_NAME:${PROJECT_NAME}>")
    if(CMAKE_BUILD_TYPE MATCHES "^(Debug|RelWithDebInfo|Release)$")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        $<TARGET_PDB_FILE:${PROJECT_NAME}> "${PLUGIN_DEPLOY_DIRECTORY}/$<TARGET_PDB_FILE_NAME:${PROJECT_NAME}>")
    endif()
endforeach()
