
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


#所有的头文件目录
include_directories ("${PROJECT_SOURCE_DIR}/sdk")
include_directories ("${PROJECT_SOURCE_DIR}/sdk/service")
include_directories ("${PROJECT_SOURCE_DIR}/sdk/third_party/cJson")
include_directories ("${PROJECT_SOURCE_DIR}/sdk/third_party/mongoose")
include_directories (${PROJECT_SOURCE_DIR}/include)
find_package(Poco REQUIRED COMPONENTS Foundation Net JSON)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(${PROJECT_NAME} yaml-cpp pthread ${Poco_LIBRARIES})

install(
    TARGETS ${PROJECT_NAME}
    # RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin
)