#!/bin/bash

./protoc --cpp_out=./ im.proto

cp /y im.pb.* server/
