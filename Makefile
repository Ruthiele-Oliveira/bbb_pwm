# --- CONFIGURAÇÃO DO PROJETO ---

# 1. Nome do programa executável
TARGET = pwm_app

# 2. Compilador
CC = arm-linux-gnueabihf-gcc

# 3. Opções de Compilação (flags para o compilador)
CFLAGS = -Wall -Wextra -g \
         -Iinclude \
         -Ilibs/bbb_gpiolib/include \
         -Ilibs/bbb_logger/include

# 3.5. Bibliotecas do Sistema para Linkar (-l<nome_da_lib>)
#      Adicionamos a biblioteca gpiod, essencial para controle de GPIO.
LDLIBS = -lgpiod


# --- DETECÇÃO AUTOMÁTICA DE ARQUIVOS ---

# 4. Fontes do projeto principal
SRCS = $(wildcard src/*.c)

# 5. Fontes das bibliotecas locais
LIB_SRCS = $(wildcard libs/bbb_gpiolib/src/*.c) \
           $(wildcard libs/bbb_logger/src/*.c)

# 6. Conversão de .c para .o
OBJS = $(SRCS:.c=.o)
LIB_OBJS = $(LIB_SRCS:.c=.o)


# --- REGRAS DE EXECUÇÃO DO MAKE ---

all: $(TARGET)

# Regra de Linkagem Final: Adicionamos $(LDLIBS) no final
$(TARGET): $(OBJS) $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIB_OBJS) $(LDLIBS)
	@echo "Programa '$(TARGET)' compilado e linkado com sucesso!"

# ... (o resto do arquivo continua igual) ...

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Limpando arquivos gerados..."
	rm -f $(TARGET) $(OBJS) $(LIB_OBJS)

run: all
	@echo "Executando o programa..."
	sudo ./$(TARGET)

.PHONY: all clean run
