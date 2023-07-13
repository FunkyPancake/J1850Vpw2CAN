cmake_minimum_required(VERSION 3.13)


project(Application LANGUAGES CXX C)


target_sources(${TARGET_NAME}
        PRIVATE
        AppTasks.cpp
        main.cpp
        GearSelector/GearSelector.cpp
        )
target_include_directories(${TARGET_NAME}
        PUBLIC
        GearSelector)



