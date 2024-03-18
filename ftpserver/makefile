OUT:=ftpserver
SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SRCS))
CC:=gcc
$(OUT):$(OBJS)
	$(CC) $^ -o $@ -g -lpthread -lcrypt -lmysqlclient -ll8w8jwt
%.o:%.c
	$(CC) -c $^ -o $@ -g -O 
clean:
	$(RM) $(OBJS) $(OUT)
rebuild:clean $(OUT)
