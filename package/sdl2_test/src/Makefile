CFLAGS ?=  -O0 -g -Wall -fmessage-length=0 -fnon-call-exceptions `sdl2-config --cflags`

#OBJS =  testdrawchessboard.c
OBJS =  sdl2-test.c

LIBS ?=  `sdl2-config --libs`

TARGET = sdl2_test

$(TARGET):      $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

all:    $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
