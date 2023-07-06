CFLAGS ?=  -O0 -g -Wall -fmessage-length=0 -fnon-call-exceptions `sdl-config --cflags`

OBJS =  sdl-test.c

LDFLAGS ?=  `sdl-config --libs`

TARGET = sdl_test

$(TARGET):      $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

all:    $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
