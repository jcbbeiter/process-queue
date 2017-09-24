CC=		g++
CFLAGS=		-g -std=c++11 -Wall
LD=		g++
LDFLAGS=	-L.
TARGETS=	pq
all:	$(TARGETS)

$(TARGETS):	pq.o config.o log.o
	@echo "Linking pq..."
	@$(LD) $(LDFLAGS) -o $@ $^

%.o:	%.cpp
	@echo "Compiling $@..."
	@$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm *.o pq
