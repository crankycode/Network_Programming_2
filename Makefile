# Note: Makefile only compatible with gnu-make
# To compile:
# make          // will compile optimised version
# make dbg=y    // will compile debug version

# Basic flags
CC = g++
CFLAGS = -Wall -pedantic -ansi -Idir -Ldir
CLIBS = -lsocket -lnsl
THREADLIBS = -lpthread
DEBUG =NILLL

all: myconfiglib.o clientReq.o server_forked.o server_threaded.o server_select.o server_single.o
	g++ -o server_forked $(CFLAGS) $(CLIBS) myconfiglib.o clientReq.o server_forked.o -D$(DEBUG)
	g++ -o server_single $(CFLAGS) $(CLIBS) myconfiglib.o clientReq.o server_single.o -D$(DEBUG)
	g++ -o server_threaded $(CFLAGS) $(CLIBS) $(THREADLIBS) myconfiglib.o clientReq.o server_threaded.o -D$(DEBUG)
	g++ -o server_select $(CFLAGS) $(CLIBS) myconfiglib.o clientReq.o server_select.o -D$(DEBUG)
	gcc $(CFLAGS) -o slow-gethttp-me gethttp.c $(CLIBS) -g -D$(DEBUG)

myconfiglib.o: myconfiglib.cpp myconfiglib.h
	g++ $(CFLAGS) $(CLIBS) -c myconfiglib.cpp myconfiglib.h -g -D$(DEBUG)

clientReq.o: clientReq.cpp clientReq.h myconfiglib.h
	g++ $(CFLAGS) $(CLIBS) -c clientReq.cpp clientReq.h myconfiglib.h -g -D$(DEBUG)
	
server_single.o: server_single.cpp utils.h clientReq.h
	g++ $(CFLAGS) $(CLIBS) -c server_single.cpp -g -D$(DEBUG)
	
server_forked.o: server_forked.cpp utils.h clientReq.h
	g++ $(CFLAGS) $(CLIBS) -c server_forked.cpp -g -D$(DEBUG)
	
server_threaded.o: server_threaded.cpp utils.h clientReq.h
	g++ $(CFLAGS) $(CLIBS) $(THREADLIBS) -c server_threaded.cpp -g -D$(DEBUG)
	
server_select.o: server_select.cpp utils.h clientReq.h
	g++ $(CFLAGS) $(CLIBS) -c server_select.cpp -g -D$(DEBUG)
	
clean:
	rm server_single *.o *.gch

request:
	./gethttp-slow yallara.cs.rmit.edu.au:40355/example.txt test1
	./gethttp-slow yallara.cs.rmit.edu.au:40355/index.html test
	./gethttp-slow yallara.cs.rmit.edu.au:40355/example.txt test2
