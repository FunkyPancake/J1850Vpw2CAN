$PROJECT="KeWjGwTcu"

mkdir build -ea 0

docker build . --no-cache --rm --file Dockerfile --tag ${PROJECT}:latest

docker run --rm -v ${PWD}:/build ${PROJECT}:latest  cp -r /project/build /build
