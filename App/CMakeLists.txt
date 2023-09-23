cmake_minimum_required(VERSION 3.25)


project(Application LANGUAGES CXX C)


target_sources(${TARGET_NAME}
        PRIVATE
        AppTasks.cpp
        main.cpp
        GearSelector/GearSelector.cpp
        StarterControl/StarterControl.cpp
        Gateway/Gateway.cpp
        J1850Vpw/J1850Vpw.cpp
        ChryslerPci/ChryslerPci.cpp

)

target_include_directories(${TARGET_NAME}
        PUBLIC
        GearSelector
        StarterControl
        Gateway
        J1850Vpw
        ChryslerPci
)



