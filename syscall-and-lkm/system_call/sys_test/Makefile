CC = gcc
OBJS = prime_sys_test.o
TARGET = prime_sys_test

all:
	make prime_sys_test

prime_sys_test: $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)
	@rm -f $(OBJS)

prime_sys_test.o: prime_sys_test.c
	$(CC) -c prime_sys_test.c


.PHONY: clean
clean:
	rm -f $(TARGET)

