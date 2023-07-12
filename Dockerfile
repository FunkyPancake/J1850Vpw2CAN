FROM mcu-compiler:latest

WORKDIR /project

RUN rm -rf /project/*

COPY App /project/Application
COPY ./GeneratedCode /project/GeneratedCode
COPY ./KeCommon /project/KeCommon
COPY ./toolchain /project/toolchain
COPY ./null.cpp /project/null.cpp
COPY ./CMakeLists.txt /project/CMakeLists.txt

RUN cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=./toolchain/toolchain.cmake
RUN cmake --build . -- -j12 > /project/build/log.txt
RUN cp *.map ./build/