OUTPUT = cearch
INPUT = ./source/main.cpp ./source/index.cpp ./source/document.cpp ./source/server.cpp ./source/session.cpp
FLAGS = -Wall -Wextra -std=c++20
LIBS = -lpugixml -lboost_system -lmupdf
MAC_INCLUDES = -I/opt/homebrew/Cellar/boost/1.86.0/include -I/usr/local/include -I/opt/homebrew/Cellar/nlohmann_json/3.10.0/include -I/opt/homebrew/Cellar/mupdf/1.23.11_1/include
MAC_LIBS = -lpugixml -L/opt/homebrew/Cellar/mupdf/1.23.11_1/lib -lmupdf -lm

build:
	clang++ $(LIBS) $(INPUT) -o $(OUTPUT) $(FLAGS)

build_mac:
	clang++ $(MAC_INCLUDES) $(INPUT) $(MAC_LIBS) -o $(OUTPUT) $(FLAGS) 

build_san:
	clang++ $(LIBS) $(INPUT) -o $(OUTPUT)_san $(FLAGS) -fsanitize=address

run: build
	./cearch ./docs.gl
