cmake_minimum_required(VERSION 3.16)
project(MretEquipment VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED  ON)




set(MY_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)


find_package(Qt6 REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        Sql
        )

add_subdirectory(src)