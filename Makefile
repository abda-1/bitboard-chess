
all:
	g++ -I src/include -I src/headers -L src/lib -o main main.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

test:
	g++ -I src/include -I src/headers -L src/lib -o test test.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

clean:
	rm -f main test