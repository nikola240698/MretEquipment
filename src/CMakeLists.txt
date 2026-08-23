# Включаем автогенераторы Qt (MOC, UIC, RCC) для исходников в этой папке
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)




# Создаем исполняемый файл
add_executable(MretEquipment
        main.cpp
        mainwindow.cpp
        inputdata.cpp
        database.cpp
        inputconnection.cpp
        inputsubstation.cpp

        ${MY_UI_DIR}/inputdata.ui
        ${MY_UI_DIR}/mainwindow.ui
        ${MY_UI_DIR}/inputconnection.ui
        ${MY_UI_DIR}/inputsubstation.ui

        ${MY_INCLUDE_DIR}/mainwindow.h
        ${MY_INCLUDE_DIR}/inputdata.h
        ${MY_INCLUDE_DIR}/database.h
        ${MY_INCLUDE_DIR}/inputconnection.h
        ${MY_INCLUDE_DIR}/inputsubstation.h
)

# Подключаем папку include, заданную в корневом CMakeLists.txt
target_include_directories(MretEquipment PRIVATE ${MY_INCLUDE_DIR} ${MY_UI_DIR})

# указываем нашему проекту корневую папку размещения проекта на диске
target_compile_definitions(MretEquipment PRIVATE
        PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
)


# Линковка компонентов Qt6
target_link_libraries(MretEquipment PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Sql

)

