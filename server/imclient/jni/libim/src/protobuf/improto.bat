@cls

@echo off

@title ****** build im proto ******

protoc --cpp_out=./ im.proto

copy /y im.pb.* server

@echo build complete

@pause