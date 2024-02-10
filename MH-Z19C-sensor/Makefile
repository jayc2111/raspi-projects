


OBJDIR = obj/
BINDIR = bin/

# Defines the RPI variable which is needed by rc-switch/RCSwitch.h
CCFLAGS = -Wall
CXXFLAGS =  -Wall

OBJ = $(OBJDIR)MH-Z19C.o $(OBJDIR)easy_log.o $(OBJDIR)main.o
#OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

$(OBJDIR)%.o: %.cpp 
	$(CXX) $(CXXFLAGS) -c $< -o $@
  
$(OBJDIR)%.o: %.c 
	$(CC) $(CCFLAGS) -c $< -o $@


all: main

main: $(OBJ)
	$(CXX) $(CXXFLAGS) $+ -o $(BINDIR)$@ 

clean:
	$(RM) $(OBJDIR)* $(BINDIR)* 

