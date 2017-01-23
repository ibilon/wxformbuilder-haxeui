cmake_minimum_required (VERSION 2.8.0)
project (wxformebuilder-haxeui)

find_package(wxWidgets COMPONENTS xrc webview html qa adv core xml net base REQUIRED)
include("${wxWidgets_USE_FILE}")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "output/lib/haxeui-editor/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "output/bin/")
set(DEFAULT_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${wxWidgets_CXX_FLAGS}")

# wxPropGrid library

set(CMAKE_CXX_FLAGS "${DEFAULT_CXX_FLAGS} -std=c++11 -MMD -D WXMAKINGDLL_PROPGRID -D MONOLITHIC -D WXUSINGDLL -D UNICODE -D _UNICODE -D __WX__ -D NDEBUG -fPIC -O3 -Wall -fPIC -fno-strict-aliasing")

file(GLOB SOURCES_propgrid src/controls/src/propgrid/**.cpp)

include_directories("src/controls/include/" ${wxWidgets_INCLUDE_DIRS})
add_library(wx_gtk2u_propgrid-3.0_wxfb SHARED ${SOURCES_propgrid})
target_link_libraries(wx_gtk2u_propgrid-3.0_wxfb ${wxWidgets_LIBRARIES})

# main

set(CMAKE_CXX_FLAGS "${DEFAULT_CXX_FLAGS} -std=c++11 -MMD -D WXUSINGDLL_NB -D TIXML_USE_TICPP -D NO_GCC_PRAGMA -D SCI_NAMESPACE -D WXUSINGDLL -D UNICODE -D _UNICODE -D __WX__ -D NDEBUG -O3 -Wall -fno-strict-aliasing")

file(GLOB_RECURSE SOURCES_main src/*.cpp src/*.cc sdk/plugin_interface/*.cpp sdk/tinyxml/*.cpp)

include_directories("src/" "src/codegen/" "src/md5/" "src/model/" "src/rad/" "src/utils" "sdk/plugin_interface/" "sdk/tinyxml/" ${wxWidgets_INCLUDE_DIRS})

add_executable(haxeui-editor ${SOURCES_main})
target_link_libraries(haxeui-editor ${wxWidgets_LIBRARIES} "wx_gtk2u_propgrid-3.0_wxfb")
