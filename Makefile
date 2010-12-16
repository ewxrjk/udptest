CXX=c++ -Wall -W

all: udplisten udpsend

udplisten: udplisten.o
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $<

udpsend: udpsend.o
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $<

clean:
	rm -f *.o
	rm -f udpsend udplisten
