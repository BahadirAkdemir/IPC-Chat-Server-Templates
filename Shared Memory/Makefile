TARGET_01        = server
SOURCE_01        = server.c

TARGET_02        = client
SOURCE_02        = client.c

CC = gcc
CFLAGS = -Wall -g3 -gdwarf-2 -DDEBUG
LDLIBS  = -lpthread -lrt

.PHONY: all
all: $(TARGET_01) $(TARGET_02)

$(TARGET_01): $(SOURCE_01)

$(TARGET_02): $(SOURCE_02)

.PHONY: clean
clean:
	rm $(TARGET_01) $(TARGET_02) $(OBJS)
