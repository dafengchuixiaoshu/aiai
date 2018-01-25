CC=g++ 

DEFINES=-D__linux__ -D_GNU_SOURCE

CFLAGS= -ldl -lcrypt -lz -lnsl  -lresolv -lpthread -lm -lrt -g

LIB=../core/lib/core.a ../protobuf/libprotobuf.a

ARCH=$(shell getconf LONG_BIT)
ifeq (${ARCH},64)
LIB += ../core/db/mysql/lib/libmysqlclient64.a
DEFINES += -D_USE_BIT64
else
LIB += ../core/db/mysql/lib/libmysqlclient.a
endif

EXE=./bin/router

INCLUDEDIR=\
        -I../core/db/src\
        -I ../core/db/mysql/include\
        -I../core/sip/include\
        -I../core/util/include\
        -I../core/ctrl\
        -I../core/http\
        -I../core/transport\
        -I../core/zlib\
        -I../core/alg\
        -I../core/im\
        -I../protobuf/server

SRC=\
	router.cpp\

OBJ=\
	${SRC:.cpp=.o}

xcapap:${SRC} ${XCAP}
	${MAKE} ${EXE}
	
all:
	-rm ${EXE} ${OBJ}
	${MAKE} ${EXE}
	

${EXE}: ${OBJ} ${LIB}
	${CC} ${CFLAGS} ${OCIFLAGS} -o ${EXE} ${OBJ} ${LIB}

clean:
	-rm ${OBJ}

.cpp.o:
	${CC} -O3 -g -w ${DEFINES} ${INCLUDEDIR} -c -o $@ $<
