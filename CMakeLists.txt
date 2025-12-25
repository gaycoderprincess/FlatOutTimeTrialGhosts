cmake_minimum_required(VERSION 3.27)
project(FlatOutTimeTrialGhosts)

SET(CMAKE_SYSTEM_NAME Windows)

SET(CMAKE_C_COMPILER i686-w64-mingw32-gcc)
SET(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)
SET(CMAKE_RC_COMPILER i686-w64-mingw32-windres)
SET(CMAKE_RANLIB i686-w64-mingw32-ranlib)

SET(CMAKE_CXX_STANDARD 20)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -masm=intel -s -static")

add_compile_definitions(IMGUI_USER_CONFIG="../../nya_imconfig.h" NYA_BACKEND_DX9 NYA_NO_FONT FLATOUT_1)
add_library(FlatOutTimeTrialGhosts SHARED
        ../nya-common/3rdparty/imgui/imgui.cpp
        ../nya-common/3rdparty/imgui/backends/imgui_impl_dx9.cpp
        ../nya-common/3rdparty/imgui/imgui_draw.cpp
        ../nya-common/3rdparty/imgui/imgui_tables.cpp
        ../nya-common/3rdparty/imgui/imgui_widgets.cpp
        ../nya-common/nya_commonfuncs.cpp
        ../nya-common/nya_commondraw.cpp
        ../nya-common/nya_dx9_hookbase.cpp
        main.cpp)
target_include_directories(FlatOutTimeTrialGhosts PUBLIC ../nya-common)
target_include_directories(FlatOutTimeTrialGhosts PUBLIC ../nya-common/3rdparty)
target_include_directories(FlatOutTimeTrialGhosts PUBLIC ../nya-common/3rdparty/imgui)
target_include_directories(FlatOutTimeTrialGhosts PUBLIC ../nya-common/3rdparty/imgui/backends)
target_include_directories(FlatOutTimeTrialGhosts PUBLIC ../nya-common-fo1)
target_include_directories(FlatOutTimeTrialGhosts PUBLIC ../FlatOut2TimeTrialGhosts)
target_include_directories(FlatOutTimeTrialGhosts PUBLIC ../CwoeeMenuLib/include)
target_link_options(FlatOutTimeTrialGhosts PRIVATE "-Wl,--exclude-all-symbols")
set_target_properties(FlatOutTimeTrialGhosts PROPERTIES PREFIX "")
set_target_properties(FlatOutTimeTrialGhosts PROPERTIES SUFFIX "_gcp.dll")

target_link_libraries(FlatOutTimeTrialGhosts d3dx9)