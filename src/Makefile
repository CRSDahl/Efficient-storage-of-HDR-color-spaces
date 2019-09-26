
CFLAGS = -O2

all: study

SRC = study.cpp

clean:
	rm -f study 

study: $(SRC)
	g++ $(CFLAGS) -I "C:\Program Files\ImageMagick-7.0.8-Q16\include" -fopenmp -DMAGICKCORE_HDRI_ENABLE=1 -DMAGICKCORE_QUANTUM_DEPTH=16 -lMagick++-7 -lMagickWand-7.Q16HDRI -lMagickCore-7.Q16HDRI -std=c++17 -lstdc++fs $(SRC)