cmake_minimum_required(VERSION 3.2)

if($ENV{TRAVIS_BUILD_NUMBER})
 project(09_homework VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})
else()
 project(09_homework VERSION 0.0.3)
endif()


include_directories(/usr/local/include)
link_directories(/usr/local/lib)
find_package(Threads)


add_executable(bayan main.cpp processor.cpp config.cpp)

set_target_properties(bayan PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(bayan PRIVATE "${CMAKE_BINARY_DIR}")
target_link_libraries(bayan PRIVATE boost_filesystem boost_system boost_program_options)
target_compile_options(bayan PRIVATE -Wall -Wextra -pedantic -Werror )


install(TARGETS bayan RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT ddzuba@yandex.ru)
include(CPack)

enable_testing()
add_test(gtests gtests)