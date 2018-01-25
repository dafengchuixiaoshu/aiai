BASE_FLAGS := \
-D__linux \
-DPOSIX \
-DXML_STATIC \
-DHAVE_EXPAT_CONFIG_H \
-DLINUX \
-I"$(PWD)\..\src\zlib-1.2.5" \
-I"$(PWD)\..\src\xmpp" \
-I"$(PWD)\..\src\xcap" \
-I"$(PWD)\..\src\sqlite" \
-I"$(PWD)\..\src\utils" \
-I"$(PWD)\..\src\libim" \
-I"$(PWD)\..\src\libclient" \
-I"$(PWD)\..\src\protobuf" \
-I"$(PWD)\..\src\protobuf\libprotobuf" \
-I"$(PWD)\..\src" \
-g -Wall -c -fmessage-length=0 -fpermissive -Wunused-function -Wunused-value 

