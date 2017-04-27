CXX		= g++
CXXFLAGS	= -g -Wall
OBJS		= lexer.o parser.o Scope.o Symbol.o Type.o
PROG		= scc

all:		$(PROG)

$(PROG):	$(OBJS)
		$(CXX) -o $(PROG) $(OBJS)

clean:;		$(RM) -f $(PROG) core *.o
