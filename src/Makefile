
CFLAGS = -O2

all: study

SRC = study.cpp

clean:
	rm -f study 

study: $(SRC)
	g++ $(CFLAGS) `Magick++-config --cppflags --cxxflags --ldflags --libs --enable-hdri` -o $@ $(SRC) -std=c++17 -lstdc++fs
