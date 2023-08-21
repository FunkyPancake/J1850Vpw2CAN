cmake_minimum_required(VERSION 3.25)


project(Application LANGUAGES CXX C)


target_sources(${TARGET_NAME}
        PRIVATE
        AppTasks.cpp
        main.cpp
        GearSelector/GearSelector.cpp
        Gateway/Gateway.cpp
        J1850Vpw/J1850Vpw.cpp
)

target_include_directories(${TARGET_NAME}
        PUBLIC
        GearSelector
        Gateway
        J1850Vpw
)



