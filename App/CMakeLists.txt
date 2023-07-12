cmake_minimum_required(VERSION 3.13)


project(Application LANGUAGES CXX C)


target_sources(${TARGET_NAME}
        PRIVATE
        App.cpp
        main.cpp
        )



