cmake_minimum_required(VERSION 3.10)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})

project(mapreduce VERSION ${PROJECT_VESRION})

find_package(Boost COMPONENTS COMPONENTS program_options REQUIRED)
find_package(Boost REQUIRED)
find_package(Threads)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(mapreduce main.cpp 
Include/Spliter.hpp
Include/Map.hpp
Include/Reduce.hpp)

set_target_properties(mapreduce PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(mapreduce PRIVATE "${CMAKE_BINARY_DIR}")
target_link_libraries(mapreduce PRIVATE ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} Boost::program_options stdc++fs)

install(TARGETS mapreduce RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)