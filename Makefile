OUTPUT = cearch
INPUT = ./source/main.cpp ./source/index.cpp ./source/document.cpp ./source/server.cpp ./source/session.cpp
FLAGS = -Wall -Wextra -std=c++20
INCLUDE = -I/opt/homebrew/Cellar/boost/1.84.0_1/include -I/usr/local/include -I/opt/homebrew/Cellar/nlohmann-json/3.11.3/include
LIBS = -lpugixml -lm

build:
	clang++ $(LIBS) $(INCLUDE) $(INPUT) -o $(OUTPUT) $(FLAGS)

run: build
	./cearch ./docs.gl
