CXX		= g++
CXXFLAGS	= -g -Wall
OBJS		= checker.o lexer.o parser_new.o Scope.o Symbol.o Type.o
PROG		= scc

all:		$(PROG)

$(PROG):	$(OBJS)
		$(CXX) -o $(PROG) $(OBJS)

clean:;		$(RM) -f $(PROG) core *.o
