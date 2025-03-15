
cmake_minimum_required(VERSION 3.15)

project(Scorched3D)

find_package(PkgConfig REQUIRED)
find_package(SDL REQUIRED)
find_package(wxWidgets REQUIRED)
find_package(Freetype REQUIRED)
find_package(expat REQUIRED)
#find_package(MySQL REQUIRED)

pkg_check_modules(FFTW3 REQUIRED fftw3)
pkg_check_modules(FFTW3F REQUIRED fftw3f)
pkg_check_modules(FFTW3L REQUIRED fftw3l)
pkg_check_modules(FFTW3Q REQUIRED fftw3q)
pkg_check_modules(OPENAL REQUIRED openal)
pkg_check_modules(ALUT REQUIRED freealut)
pkg_check_modules(OGG REQUIRED ogg)
pkg_check_modules(GLEW REQUIRED glew)
pkg_check_modules(GL REQUIRED gl)
pkg_check_modules(PNG REQUIRED libpng)
pkg_check_modules(JPEG REQUIRED libjpeg)
# pkg_check_modules(MYSQL REQUIRED MySQL)

add_compile_definitions(HAVE_VASPRINTF HAVE_SNPRINTF HAVE_VSNPRINTF)

#   Launcher

add_executable(scorched3d)

set_target_properties(scorched3d PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(scorched3d PUBLIC /usr/include)
target_include_directories(scorched3d PUBLIC ${SDL_INCLUDE_DIRS})
target_include_directories(scorched3d PUBLIC ${wxWidgets_INCLUDE_DIRS})
target_include_directories(scorched3d PUBLIC ${FREETYPE_INCLUDE_DIRS})

execute_process(COMMAND wx-config --cxxflags OUTPUT_VARIABLE wx_cxx_flags ECHO_OUTPUT_VARIABLE)
set_target_properties(scorched3d PROPERTIES COMPILE_FLAGS ${wx_cxx_flags})

add_subdirectory(src/launcher/scorched/)

target_include_directories(scorched3d PUBLIC
    src/common/porting
    src/common
    src/client
    src/server
    src/launcher
)

target_link_libraries(scorched3d SDL SDL_net ${wxWidgets_LIBRARIES} freetype expat z)


#   Client

add_executable(scorchedc)

add_subdirectory(src/client/scorchedc/)

target_include_directories(scorchedc PUBLIC ${FREETYPE_INCLUDE_DIRS})
target_include_directories(scorchedc PUBLIC ${FFTW_INCLUDE_DIRS})
target_include_directories(scorchedc PUBLIC ${OPENAL_INCLUDE_DIRS})
target_include_directories(scorchedc PUBLIC ${ALUT_INCLUDE_DIRS})
target_include_directories(scorchedc PUBLIC ${OGG_INCLUDE_DIRS})
target_include_directories(scorchedc PUBLIC ${GLEW_INCLUDE_DIRS})
target_include_directories(scorchedc PUBLIC
    /usr/include
    src/common/porting
    src/common
    src/client
    src/server
)

target_link_libraries(scorchedc
    ${FFTW3_LIBRARIES}
    ${FFTW3F_LIBRARIES}
    ${OPENAL_LIBRARY}
    ${ALUT_LIBRARIES}
    ${OGG_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${GL_LIBRARIES}
    ${PNG_LIBRARIES}
    ${JPEG_LIBRARIES}
    SDL
    SDL_net
    freetype
    expat
    alut
    z
)

#   Server

add_executable(scorcheds)

add_subdirectory(src/server/scorcheds/)

target_include_directories(scorcheds PUBLIC
    src/common/porting
    src/common
    src/server
)

target_compile_definitions(scorcheds PUBLIC S3D_SERVER=1)

target_link_libraries(scorcheds
    SDL
    SDL_net
    expat
    ${JPEG_LIBRARIES}
    ${PNG_LIBRARIES}
    z
)
