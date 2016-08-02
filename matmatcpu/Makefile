CCFLAGS	:= -O3 -std=c99

all: matmat


matmat: matmat.c common.o
	$(CC) $(CCFLAGS) $^ -o $@


matmat.o: matmat.c
	$(CC) $(CCFLAGS) -c $< -o $@


%.o: %.c %.h
	$(CC) $(CCFLAGS) -c $< -o $@


clean:
	@rm -f matmat
	@rm -f *.o


@PHONY: all clean
