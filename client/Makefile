# Nombre del ejecutable
TARGET = client.exe

# Compilador (sin comillas)
CC = C:\msys64\ucrt64\bin\gcc.exe

# Archivos fuente
SRCS = client.c socketManager.c

# Flags del compilador
CFLAGS = -Wall -g

# Librerías necesarias para sockets en Windows
LIBS = -lws2_32

# Regla por defecto
all: $(TARGET)

# Cómo construir el ejecutable directamente desde los .c
$(TARGET):
	$(CC) $(SRCS) $(CFLAGS) $(LIBS) -o $(TARGET)
