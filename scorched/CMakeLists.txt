
cmake_minimum_required(VERSION 3.15)

project(Scorched3D)

find_package(SDL REQUIRED)
find_package(wxWidgets REQUIRED)
find_package(Freetype REQUIRED)
find_package(expat REQUIRED)

add_executable(scorched3d)

set_target_properties(scorched3d PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(scorched3d PUBLIC /usr/include)
target_include_directories(scorched3d PUBLIC ${SDL_INCLUDE_DIRS})
target_include_directories(scorched3d PUBLIC ${wxWidgets_INCLUDE_DIRS})
target_include_directories(scorched3d PUBLIC ${FREETYPE_INCLUDE_DIRS})
message("Freetype include dirs:" ${FREETYPE_INCLUDE_DIRS})
execute_process(COMMAND wx-config --cxxflags OUTPUT_VARIABLE wx_cxx_flags ECHO_OUTPUT_VARIABLE)
# message(${wx_cxx_flags})
set_target_properties(scorched3d PROPERTIES COMPILE_FLAGS ${wx_cxx_flags})
#    "-I/usr/lib/wx/include/gtk3-unicode-3.2 -I/usr/include/wx-3.2 -DWXUSINGDLL -D__WXGTK3__ -D__WXGTK__ -D_FILE_OFFSET_BITS=64 ")

add_subdirectory(src/launcher/scorched/)

target_include_directories(scorched3d PUBLIC
    src/common/porting
    src/common
    src/client
    src/server
    src/launcher
)


add_compile_definitions(HAVE_VASPRINTF HAVE_SNPRINTF HAVE_VSNPRINTF)

# message("wxWidgets libraries:" ${wxWidgets_LIBRARIES})
# message("expat libraries:" ${expat_LIBRARIES})

target_link_libraries(scorched3d SDL SDL_net ${wxWidgets_LIBRARIES} freetype expat z)

#get_target_property(scorched3d_srcs scorched3d SOURCES)

#message(Sources: ${scorched3d_srcs})
