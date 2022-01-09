CC = gcc
OBJS = prime_user.o
TARGET = prime_user

all:
	make prime_user

prime_user: $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)
	@rm -f $(OBJS)

prime_user.o: prime_user.c
	$(CC) -c prime_user.c


.PHONY: clean
clean:
	rm -f $(TARGET)
