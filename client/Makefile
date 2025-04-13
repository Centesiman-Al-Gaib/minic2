TARGET = client.exe
CC = C:\msys64\ucrt64\bin\gcc.exe
SRCS = client.c SocketManager.c .\importer\Importer.c .\crypto\Crypto.c
CFLAGS = -Wall -g
LIBS = -lws2_32

all: $(TARGET)

$(TARGET):
	$(CC) $(SRCS) $(CFLAGS) $(LIBS) -o $(TARGET)

definitions:
	$(CC) GenerateDefinition.c .\crypto\Crypto.c -o GenerateDefintion.exe

clean:
	powershell.exe -Command	"erase *.exe"