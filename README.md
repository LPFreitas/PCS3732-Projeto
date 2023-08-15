# **PCS3732 - Projeto JEMAPELI**

## **Compilador de Expressões Aritméticas**

### <u>Integrantes do grupo</u>:

- Lucas Palmiro de Freitas (11803396)
- Vinícius Barros Alvarenga (11257564)
- Yuri de Sene Alvizi (11260398)

### <u>Passo-a-passo de execução</u>:

1. Insira no arquivo `input.txt` a expressão aritmética de operandos inteiros (em binário de no máximo 12 bits) com parênteses **'('** e **')'** e com os operadores **'+'**, **'-'**, **'\*'** e **'/'**, mantendo os operandos separados dos operadores por espaços **' '**.

2. Para limpar os arquivos de saída e executáveis, execute:

```
make clean
```

3. Para compilar o código do compilador `compiler.c`, executá-lo e, assim, gerar o código _assembly_ ARMv7 correspondente à expressão aritmética fornecida no arquivo `output.s`, execute o seguinte comando no terminal:

```
make
```

4. Em seguida, para iniciar o emulador QEMU, execute o comando:

```
make qemu
```

5. Por fim, para executar o código _assembly_ ARMv7 gerado no depurador GDB, execute:

```
make gdb
```

O resultado da execução do código _assembly_ ARMv7 está no registrador **r0**.
