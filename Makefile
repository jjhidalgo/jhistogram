OBJDIR = .
PROGDIR = .

PROG =	$(addprefix $(PROGDIR)/, jhistogram)

COMMONS = 
SRCS =	hist.cpp
OBJS =	$(addprefix $(OBJDIR)/, hist.o) 

LIBS =	-lgsl -lgslcblas -lm 
#PARLL = -fopenmp
PARLL = 
CPP = g++
#Debug
CPPFLAGS = -ggdb -Wall 
CPPFLAGS = -g3 
#Release
CPPFLAGS = -O3
LDFLAGS = 

.SUFFIXES : .o .cpp

all: $(PROG)

# Regla implícita para pasar .cpp a .o (forma nueva "patern rule").
$(OBJDIR)/%.o : %.cpp
	$(CPP) -c $(CPPFLAGS) $(PARLL) $(LIBS)  $< -o $@

$(PROG): $(OBJS)
	$(CPP) $(PARLL) $(CPPFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

cleanobj:
	rm -f $(OBJS)
clean:
	rm -f $(PROG) $(OBJS)



