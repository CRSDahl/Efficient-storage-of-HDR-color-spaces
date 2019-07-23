
CFLAGS = -O2

all: study

SRC = study.cpp

clean:
	rm -f study 

study: $(SRC)
	g++ $(CFLAGS) `Magick++-config --cppflags --cxxflags --ldflags --libs`  -o $@ $(SRC)
