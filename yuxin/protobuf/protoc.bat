@cls

@echo off

@title ****** build protobuf ******

build\protoc --cpp_out=./ ./mzim/mzim.proto

@echo build complete

@pause