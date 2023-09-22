
# #加载指定目录下的指定格式文件集
# file(GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR}/sdk/*.cpp  ${PROJECT_SOURCE_DIR}/sdk/*.c  ${PROJECT_SOURCE_DIR}/src/main.cpp)
# file(GLOB_RECURSE HEADER_FILES  ${PROJECT_SOURCE_DIR}/sdk/*.h  ${PROJECT_SOURCE_DIR}/sdk/*.hpp)

# source_group("Include" FILES ${HEADER_FILES})
# #编译生成可执行文件${PROJECT_NAME}
# my_add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
# #添加链接库pthread
# target_link_libraries(${PROJECT_NAME} m pthread ${Poco_LIBRARIES})

# force C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#设置生成的VS项目文件结构和本地文件目录一样
function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)
 
function(my_add_executable)
    foreach(_source IN ITEMS ${ARGN})
        assign_source_group(${_source})
    endforeach()
    add_executable(${ARGV})
endfunction(my_add_executable)

# 添加对gdb的支持
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

#指定生成debug模式
set(CMAKE_CONFIGURATION_TYPES Debug)

#所有的头文件目录
include_directories (${PROJECT_SOURCE_DIR}/sdk)
include_directories (${PROJECT_SOURCE_DIR}/sdk/service)
include_directories (${PROJECT_SOURCE_DIR}/sdk/third_party/cJson)
include_directories (${PROJECT_SOURCE_DIR}/sdk/third_party/mongoose)
include_directories (${PROJECT_SOURCE_DIR}/include)

#加载指定目录下的指定格式文件集
file(GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR}/sdk/*.cpp  ${PROJECT_SOURCE_DIR}/sdk/*.c  ${PROJECT_SOURCE_DIR}/src/main.cpp)
file(GLOB_RECURSE HEADER_FILES  ${PROJECT_SOURCE_DIR}/sdk/*.h  ${PROJECT_SOURCE_DIR}/sdk/*.hpp)

find_package(Poco REQUIRED COMPONENTS Foundation Net JSON)

source_group("Include" FILES ${HEADER_FILES})
#编译生成可执行文件${PROJECT_NAME}
my_add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
#添加链接库pthread
target_link_libraries(${PROJECT_NAME} m pthread ${Poco_LIBRARIES} fmt)

install(
    TARGETS ${PROJECT_NAME}
    # RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin
)