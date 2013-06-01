CC = gcc
CFLAGS = -gstabs -pedantic -W -Wall -std=gnu99

OBJS = main.o stringUtils.o inputHandling.o syscallsWrapper.o processHandling.o signalHandling.o 

simpleShell: $(OBJS)
	$(CC) $(CFLAGS) -o sish $(OBJS) 


main.o: main.c
	$(CC) $(CFLAGS) -c main.c

inputHandling.o: inputHandling.c
	$(CC) $(CFLAGS) -c inputHandling.c

processHandling.o: processHandling.c
	$(CC) $(CFLAGS) -c processHandling.c

syscallsWrapper.o: syscallsWrapper.c
	$(CC) $(CFLAGS) -c syscallsWrapper.c
 
signalHandling.o: signalHandling.c
	$(CC) $(CFLAGS) -c signalHandling.c

stringUtils.o: stringUtils.c
	$(CC) $(CFLAGS) -c stringUtils.c
clean:

	-rm sish *.o
