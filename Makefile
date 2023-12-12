SRC = Engine/
GFLAGS = -Wall -O3 -fopenmp -flto -funroll-loops

main: $(SRC)main.o $(SRC)Board.o $(SRC)engine.o
	g++ $(GFLAGS) -o main $(SRC)main.o $(SRC)Board.o $(SRC)engine.o
	copy main.exe /D TestChessGame\Assets\Plugins\engine.exe

$(SRC)main.o: $(SRC)main.cpp $(SRC)Board.h $(SRC)engine.hpp
	g++ $(GFLAGS) -c $(SRC)main.cpp -o $(SRC)main.o

$(SRC)Board.o: $(SRC)Board.c $(SRC)Board.h $(SRC)ParseFen.c
	g++ $(GFLAGS) -c $(SRC)Board.c -o $(SRC)Board.o

$(SRC)engine.o: $(SRC)engine.cpp $(SRC)engine.hpp $(SRC)Board.h $(SRC)PieceScores.h
	g++ $(GFLAGS) -c $(SRC)engine.cpp -o $(SRC)engine.o

ifdef OS
   RM = del /Q
   FixPath = $(subst /,\,$1)
else
   ifeq ($(shell uname), Linux)
	  RM = rm -f
	  FixPath = $1
   endif
endif

clean:
	$(RM) $(call FixPath, $(SRC)*.o)
