# compiler
CC = g++

# compiler flags:
CFLAGS  = -g -Wall

# build target executable:
TARGET = Syn_Anal
OUTPUT_NAME = scc
all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) -o $(OUTPUT_NAME) $(TARGET).cpp

clean:
	$(RM) $(TARGET) $(OUTPUT_NAME)
