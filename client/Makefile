TARGET = client.exe
CC = C:\msys64\ucrt64\bin\gcc.exe
SRCS = client.c .\message\Message.c .\message\MessageQueue.c .\socketManager\SocketManager.c .\importer\Importer.c .\crypto\Crypto.c .\memoryCollector\MemoryCollector.c
CFLAGS = -Wall -g 
LIBS = -lws2_32 -lcrypt32

all: $(TARGET)

$(TARGET):
	$(CC) $(SRCS) $(CFLAGS) $(LIBS) -o $(TARGET)

def:
	$(CC) GenerateDefinition.c .\crypto\Crypto.c $(CFLAGS) $(LIBS) -o GenerateDefintion.exe

clean:
	powershell.exe -Command	"erase *.exe"