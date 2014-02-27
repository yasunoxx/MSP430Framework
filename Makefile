# makefile configuration
NAME = main
OBJS = main.o io.o lcd.o tlv.o
ARCH  = msp430
CPU = msp430g2452

CFLAGS = -mmcu=${CPU} -O2 -Wall -g

#switch the compiler (for the internal make rules)
CC = ${ARCH}-gcc
OBJCOPY = ${ARCH}-objcopy
OBJDUMP = ${ARCH}-objdump

.PHONY: all FORCE clean download  dist

#all should be the first target. it's built when make is run without args
all: ${NAME}.elf ${NAME}.a43 ${NAME}.lst

#additional rules for files
${NAME}.elf: ${OBJS}
	${CC} -mmcu=${CPU} -o $@ ${OBJS}

${NAME}.a43: ${NAME}.elf
#	${OBJCOPY} -O ihex $^ $@
	$(OBJCOPY) -O srec $^ $@

${NAME}.lst: ${NAME}.elf
	${OBJDUMP} -dSt $^ >$@

clean:
	rm -f ${NAME}.elf ${NAME}.a43 ${NAME}.lst ${OBJS} core

#backup archive
dist:
	tar czf dist.tgz *.c *.h *.txt makefile

#dummy target as dependecy if something has to be build everytime
FORCE:
