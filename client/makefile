OUT:=client
SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SRCS))
CC:=gcc
$(OUT):$(OBJS)
	$(CC) $^ -o $@ -g -lpthread -lcrypt -ll8w8jwt
%.o:%.c
	$(CC) -c $^ -o $@ -g -O0 
clean:
	$(RM) $(OBJS) $(OUT)
rebuild:clean $(OUT)
