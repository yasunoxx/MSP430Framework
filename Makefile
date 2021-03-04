# makefile configuration
NAME = main
OBJS = main.o io.o lcd.o tlv.o
ARCH  = msp430-elf
CPU = msp430g2553
# set your tool path and select GCC version
TOOLPATH = /usr/local/msp430-gcc
# GCC_VERSION_463 = true
GCC_VERSION_463 =


ifdef GCC_VERSION_463
CFLAGS = -mmcu=${CPU} -O2 -Wall -g
else
CFLAGS = -mmcu=${CPU} -O2 -Wall -g -I${TOOLPATH}/include
endif

#switch the compiler (for the internal make rules)
CC = ${ARCH}-gcc
OBJCOPY = ${ARCH}-objcopy
OBJDUMP = ${ARCH}-objdump

.PHONY: all FORCE clean download  dist

#all should be the first target. it's built when make is run without args
all: ${NAME}.elf ${NAME}.a43 ${NAME}.lst

#additional rules for files
${NAME}.elf: ${OBJS}
ifdef GCC_VERSION_463
	${CC} -mmcu=${CPU} -o $@ ${OBJS}
else
	${CC} -mmcu=${CPU} -o $@ ${OBJS} -L${TOOLPATH}/include
endif

${NAME}.a43: ${NAME}.elf
#	${OBJCOPY} -O ihex $^ $@
	$(OBJCOPY) -O srec $^ $@

${NAME}.lst: ${NAME}.elf
	${OBJDUMP} -dSt $^ >$@

clean:
	rm -f ${NAME}.elf ${NAME}.a43 ${NAME}.lst ${OBJS} core

gitpush:
	git push -u origin master

#backup archive
dist:
	tar czf dist.tgz *.c *.h *.txt makefile

#dummy target as dependecy if something has to be build everytime
FORCE:
