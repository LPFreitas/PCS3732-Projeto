# Makefile
FONTE_AS = output.s
FONTE_C = compiler.c

# Arquivos de saída 
EXEC_AS = kernel.elf
EXEC_C = compiler
MAP = kernel.map

PREFIXO = arm-none-eabi-
LDSCRIPT = kernel.ld
AS = ${PREFIXO}as
LD = ${PREFIXO}ld
OBJETO_AS = $(FONTE_AS:.s=.o)

all: ${EXEC_C} ${FONTE_AS} ${EXEC_AS}

# Gerar executável C
${EXEC_C}: ${FONTE_C}
	gcc $(FONTE_C) -o ${EXEC_C}

# Executar executável C
${FONTE_AS}: ${EXEC_C}
	./${EXEC_C}

# Gerar executável AS
${EXEC_AS}: ${OBJETO_AS} ${EXEC_C}
	${LD} -T ${LDSCRIPT} -M=${MAP} -o $@ $<

# Montar arquivos em assembler
.s.o:
	${AS} -g -o $@ $<

# Limpar tudo
clean:
	rm -f ${FONTE_AS} ${OBJETO_AS} ${EXEC_AS} ${EXEC_C} ${MAP}

# Iniciar qemu
qemu: ${EXEC_AS}
	@if lsof -Pi :1234 >/dev/null ; then\
		echo "qemu já está executando"; \
	else qemu-system-arm -s -M virt -kernel ${EXEC_AS} & \
	fi

# Iniciar gdb
gdb: ${EXEC_AS}
	gdb-multiarch -ex "set architecture arm" \
		          -ex "target extended-remote :1234" \
				  -ex "load" \
				  -ex "layout src" \
				  -ex "layout regs" \
				  -ex "b end" \
				  -ex "c" \
				  ${EXEC_AS}