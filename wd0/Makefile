SHELL=/bin/sh
BENCHMARK=wc
BENCHMARKU=WC

include ../config/make.def

OBJS = wc.o ${COMMON}/timer.o

include ../sys/make.common

${PROGRAM}: config ${OBJS}
	${CLINK} ${CLINKFLAGS} -o ${PROGRAM} ${OBJS} ${C_LIB}


wc.o:		wc.cpp 
	${CCOMPILE} wc.cpp timer.cpp

clean:
	- rm -f *.o *~