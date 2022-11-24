if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(LIB_STATIC_EXT ".a")
    set(LIB_STATIC_EXT ".dylib")
    set(LIB_PREFIX "lib")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(LIB_STATIC_EXT ".a")
    set(LIB_SHARED_EXT ".so")
    set(LIB_PREFIX "lib")

    if (TARGET_ARCH MATCHES "x64")
        set(LIB_STATIC_SUFFIX "")
    else()
        set(LIB_STATIC_SUFFIX "_i486")
    endif()
elseif(WIN32)
    set(LIB_STATIC_EXT ".lib")
    set(LIB_SHARED_EXT ".dll")
    set(LIB_STATIC_SUFFIX "")
    # If have WIN64 then do further things
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(ENABLE_OPTIMIZE ON)
    set(ENABLE_DEBUG OFF)
else()
    set(ENABLE_OPTIMIZE OFF)
    set(ENABLE_DEBUG ON)
endif()

if (SOURCEMOD_PATH STREQUAL "")
	set(SOURCEMOD_PATH $ENV{SOURCEMOD})
endif()

string(REPLACE "\\" "/" SOURCEMOD_PATH ${SOURCEMOD_PATH})

if (METAMOD_PATH STREQUAL "")
	set(METAMOD_PATH $ENV{METAMOD_SOURCE})
endif()

string(REPLACE "\\" "/" METAMOD_PATH ${METAMOD_PATH})

add_library(mmsdk INTERFACE)
target_include_directories(mmsdk INTERFACE
	${METAMOD_PATH}/core
	${METAMOD_PATH}/public
	${METAMOD_PATH}/public/sourcehook
)

add_definitions(-DMETA_NO_HL2SDK)

add_library(amtl INTERFACE)
target_include_directories(amtl INTERFACE
    ${SOURCEMOD_PATH}/public/amtl
    ${SOURCEMOD_PATH}/public/amtl/amtl
)

add_library(smsdk INTERFACE)
target_include_directories(smsdk INTERFACE
    ${SOURCEMOD_PATH}/core
    ${SOURCEMOD_PATH}/public
    ${SOURCEMOD_PATH}/sourcepawn/include
    )
target_link_libraries(smsdk INTERFACE amtl)

if (ENABLE_DEBUG)
    add_definitions(-D_DEBUG -DDEBUG)
endif()

if (ENABLE_OPTIMIZE)
    add_definitions(-DNDEBUG)
endif()

if (WIN32)
    add_definitions(-DWIN32 -D_WINDOWS)
elseif (UNIX)
    add_definitions(-DLINUX -D_LINUX)
    string(APPEND CMAKE_SHARED_LINKER_FLAGS
        " -Wl,--exclude-libs,ALL"
        " -lm"
        " -lstdc++"
    )
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU") #gcc
        string(APPEND CMAKE_SHARED_LINKER_FLAGS
            " -static-libgcc"
        )
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS
            " -lgcc_eh"
        )
    endif()
endif()

if (MSVC)
    add_compile_options(/wd4819 /wd4828 /wd5033 /permissive- /utf-8 /wd4005)
    
    # Don't be lazy :)
    if (ENABLE_DEBUG)
        string(APPEND CMAKE_C_FLAGS
            " /MTd"
        )
        string(APPEND CMAKE_SHARED_LINKER_FLAGS
            " /NODEFAULTLIB:libcmt"
        )
    else ()
        string(APPEND CMAKE_C_FLAGS
            " /MT"
        )
    endif()
    add_compile_definitions(
        _CRT_SECURE_NO_WARNINGS=1 _CRT_SECURE_NO_DEPRECATE=1
        _CRT_NONSTDC_NO_DEPRECATE=1 _ITERATOR_DEBUG_LEVEL=0
    )
    string(APPEND CMAKE_C_FLAGS
        " /W3"
    )
    # /EHsc has already been included
    string(APPEND CMAKE_CXX_FLAGS
        " /GR-"
        " /TP"
    )
    
    if (ENABLE_OPTIMIZE)
        string(APPEND CMAKE_C_FLAGS
            " /Ox"
            " /Zo"
        )
        string(APPEND CMAKE_SHARED_LINKER_FLAGS
            " /OPT:ICF"
            " /OPT:REF"
        )
    endif()

    if (ENABLE_DEBUG)
        string(APPEND CMAKE_C_FLAGS
            " /Od"
            " /RTC1"
        )
    endif()
    
    string(APPEND CMAKE_C_FLAGS
        " /Oy-"
    )
    
    string(APPEND CMAKE_SHARED_LINKER_FLAGS
        " /MACHINE:x86"
    )
    
    add_compile_definitions(COMPILER_MSVC COMPILER_MSVC32)
else()
    # Work for both
    add_definitions(
        -Dstricmp=strcasecmp -D_stricmp=strcasecmp
        -D_snprintf=snprintf -D_vsnprintf=vsnprintf
        -DHAVE_STDINT_H -DGNUC -DCOMPILER_GCC
    )
    # for better visibility, we separate it onto two lines
    string(APPEND CMAKE_C_FLAGS
        " -pipe"
        " -fno-strict-aliasing"
        " -Wall"
        " -Werror"
    )
    string(APPEND CMAKE_C_FLAGS
        " -Wno-unused"
        " -Wno-switch"
        " -Wno-array-bounds"
        " -msse"
        " -fvisibility=hidden"
    )
    if (TARGET_ARCH STREQUAL "x86")
        string(APPEND CMAKE_C_FLAGS
            " -m32"
        )
        string(APPEND CMAKE_SHARED_LINKER_FLAGS
            " -m32"
        )

    endif()
    string(APPEND CMAKE_CXX_FLAGS
        " -fno-exceptions"
        " -fno-threadsafe-statics"
        " -Wno-non-virtual-dtor"
        " -Wno-overloaded-virtual"
        " -fvisibility-inlines-hidden"
        " -Wno-delete-non-virtual-dtor"
    )
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU") #gcc
        string(APPEND CMAKE_C_FLAGS
            " -Wno-narrowing"
            " -Wno-unused-result"
            " -mfpmath=sse"
        )
        if (ENABLE_OPTIMIZE)
            string(APPEND CMAKE_C_FLAGS
                " -O3"
            )
        endif()
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        string(APPEND CMAKE_C_FLAGS
            " -Wno-sometimes-uninitialized"
        )
        string(APPEND CMAKE_CXX_FLAGS
            " -Wno-inconsistent-missing-override"
            " -Wno-implicit-exception-spec-mismatch"
            " -Wno-deprecated-register"
        )
    endif()
endif()

add_library(smsdk_ext INTERFACE)
target_sources(smsdk_ext INTERFACE ${SOURCEMOD_PATH}/public/smsdk_ext.cpp)
target_compile_definitions(smsdk_ext INTERFACE -DSDK_EXPORTS -DSOURCEMOD_BUILD)
target_link_libraries(smsdk_ext INTERFACE smsdk mmsdk)

add_library(amsdk INTERFACE)
target_link_libraries(amsdk INTERFACE smsdk_ext)