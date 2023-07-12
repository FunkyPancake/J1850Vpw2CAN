cmake_minimum_required(VERSION 3.25)
set(TARGET_NAME KeWjGwTcu)

project(${TARGET_NAME} LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_BUILD_TYPE MATCHES Debug)

    add_executable(${TARGET_NAME}
            GeneratedCode/board/peripherals.c
            GeneratedCode/board/clock_config.c
            GeneratedCode/board/pin_mux.c)

    target_include_directories(${TARGET_NAME}
            PUBLIC
            GeneratedCode/board)

    add_subdirectory(KeCommon)

    target_link_libraries(${TARGET_NAME} KeCommon)

    target_compile_options(${TARGET_NAME} PRIVATE
            -O0 -fno-common -g3 -Wall -c -ffunction-sections -fno-rtti -fdata-sections -ffreestanding
            -fno-builtin -fno-exceptions -fmerge-constants -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
            -mthumb -fstack-usage -specs=nano.specs)

    target_compile_definitions(${TARGET_NAME}
            PRIVATE
            CPU_MKE16F256VLH16
            CPU_MKE16F256VLH16_cm4
            FSL_RTOS_FREE_RTOS
            SDK_OS_FREE_RTOS
            SDK_DEBUGCONSOLE=1
            __MCUXPRESSO
            __USE_CMSIS
            DEBUG
            __NEWLIB__
            )
    target_link_directories(${TARGET_NAME} PRIVATE KeCommon/nxp/linker)
    target_link_options(${TARGET_NAME} PRIVATE -Wl,-print-memory-usage,--sort-section=alignment,--gc-sections,--cref,-M=${TARGET_NAME}.map -nostdlib -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -T Ke16F256.ld)

    set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME ${TARGET_NAME}.elf)
    #    set_target_properties(${TARGET_NAME}
    #        PROPERTIES RUNTIME_OUTPUT_DIRECTORY ./build)


elseif (CMAKE_BUILD_TYPE MATCHES Test)
    FindGTest()
    if (NOT GTest_FOUND)
        include(FetchContent)

        FetchContent_Declare(
                googletest
                URL https://github.com/google/googletest/archive/refs/tags/release-1.10.0.zip
        )
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
    endif ()
    enable_testing()

    add_executable(
            Utest
            null.cpp
    )
    target_include_directories(
            Utest
            PUBLIC
            KeCommon/Interfaces
    )
    target_link_libraries(
            Utest
            gtest_main
            gmock
    )
    include(GoogleTest)

    gtest_discover_tests(Utest)
endif ()
add_subdirectory(App)

