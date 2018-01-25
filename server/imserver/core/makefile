CC=g++ 

DEFINES=-D__linux__ -D_GNU_SOURCE -D_USE_QUEUE -D_USE_EPOLL2 

CFLAGS=-ldl -lcrypt -g

OCIFLAGS=-lclntsh -Qy -lc -lm

ARCH=$(shell getconf LONG_BIT)
ifeq ($(ARCH),64)
DEFINES += -D_USE_BIT64
endif

INCLUDEDIR=\
	-I./ctrl\
	-I./db/src\
	-I./util/include\
	-I./http\
	-I./ctrl/include\
	-I./transport\
	-I./zlib\
    -I../protobuf/server\
    -I./im

ROOT=\
	ctrl/main.cpp

DB=\
	db/src/mysqlagent.cpp   db/src/DBManager.cpp
	
UTIL=\
	util/source/ResourceBase.cpp        util/source/xml.cpp\
	util/source/Common.cpp              util/source/vmd5.cpp

CTRL=\
	ctrl/BaseModule.cpp        ctrl/Log.cpp\
	ctrl/Config.cpp            ctrl/Control.cpp\
	ctrl/dbp_decrypt.cpp       ctrl/baseparam.cpp\
	ctrl/PublicFunc.cpp        ctrl/hashfun.cpp

TRANSPORT=\
	transport/udpserver.cpp    transport/tcpsocket.cpp\
	transport/tcpserver.cpp    transport/tcpclient.cpp\

HTTP=\
	http/httpserver.cpp         http/httpmsg.cpp\
	http/httpclient.cpp

IM=\
    im/imsocket.cpp

ALG=\
	alg/aes.cpp\
	alg/base64.cpp\

zlib=\
	zlib/zlibgzip.cpp

protobuf=\
    ../protobuf/im.pb.cc
    
SRC=\
	${ROOT}\
	${DB}\
	${UTIL}\
	${CTRL}\
	${TRANSPORT}\
	${HTTP}\
	${ALG}\
    ${IM}\
    ${protobuf}\
    ${zlib}
	
OBJ=\
	${ROOT:.cpp=.o}\
	${DB:.cpp=.o}\
	${UTIL:.cpp=.o}\
	${CTRL:.cpp=.o}\
	${HTTP:.cpp=.o}\
	${TRANSPORT:.cpp=.o}\
	${HTTP:.cpp=.o}\
	${ALG:.cpp=.o}\
	${zlib:.cpp=.o}\
    ${protobuf:.cc=.o}\
	${IM:.cpp=.o}

LIB=./lib/core.a

lib:${SRC}
	echo ${DEFINES} 
	${MAKE} ${LIB}

all:
	-rm ${LIB} ${OBJ}
	${MAKE} ${LIB}

	
${LIB}:${OBJ}
	ar -r ${LIB} ${OBJ} 

clean:
	-rm ${OBJ}

.cpp.o:${SRC}
	${CC} ${CFLAGS} ${DEFINES} ${INCLUDEDIR} -c -o $@ $<

.cc.o:${SRC}
	${CC} ${CFLAGS} ${DEFINES} ${INCLUDEDIR} -c -o $@ $<    