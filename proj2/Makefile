CC=gcc
OBJS=20170301.o
TARGET=20170301.out

$(TARGET): $(OBJS)
	$(CC) -Wall -o $@ $(OBJS)


20170301.o : 20170301.c 20170301.h

clean : 
	rm $(OBJS)
	rm $(TARGET)
	rm -f *.gch
