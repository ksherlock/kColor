CC	= cc
CXX	= cc
CXXFLAGS	= -W -O3
LDLIBS	= -lbe 

TARGET	= kColor

# for compiling
OBJS	= main.o window.o ColorControl.o ColorView.o ColorWell.o

.SUFFIXES: .cpp

.cc.o:
	$(CXX) -c $(CXXFLAGS) $<


$(TARGET): $(OBJS) 
	$(CC) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $@
	xres -o $@ kColor.rsrc 

clean:
	$(RM) *.o 

clobber: clean
	$(RM) -f $(TARGET)
               
main.o:		main.cc window.h
window.o:	ColorControl.h ColorView.h ColorWell.h
ColorView.o:	ColorView.h
ColorControl.o:	ColorControl.h
ColorWell.o:	ColorWell.h
