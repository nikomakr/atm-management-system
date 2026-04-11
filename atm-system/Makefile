CC      = cc
CFLAGS  = -Wall -Wextra -std=c11
LDFLAGS = -lsqlite3 -framework CoreFoundation -framework Security
TARGET  = atm
SRCS    = src/main.c src/system.c src/auth.c src/notification.c src/db.c
OBJS    = $(SRCS:.c=.o)

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

src/%.o : src/%.c src/header.h
	$(CC) $(CFLAGS) -c -o $@ $<

run : $(TARGET)
	./$(TARGET)

clean :
	rm -f $(OBJS) $(TARGET)

.PHONY : run clean
