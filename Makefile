CC = c99

OBJS = main.o

all: bin/foxstd-memins

bin/foxstd-memins: $(OBJS)
	@mkdir -p bin
	$(CC) -o $@ $(OBJS)

clean:
	rm -rf bin $(OBJS)

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

.SUFFIXES: .c
