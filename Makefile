OUTPUT = cearch
SOURCE = 	./source/main.cpp \
			./source/Index.cpp \
			./source/Server.cpp \
			./source/Session.cpp \
			./source/Document.cpp \
			./source/XMLDocument.cpp \
			./source/TextDocument.cpp

FLAGS = -Wall -Wextra -std=c++20
LIBS = -lpugixml -lboost_system -lpoppler-cpp

MAC_INCLUDES =  -I/opt/homebrew/Cellar/boost/1.86.0/include \
				-I/opt/homebrew/Cellar/poppler/24.04.0_1/include \
				-L/opt/homebrew/Cellar/poppler/24.04.0_1/lib/ 
MAC_LIBS = -lpugixml -lpoppler-cpp

build:
	clang++ $(LIBS) $(SOURCE) -o $(OUTPUT) $(FLAGS)

build_mac:
	clang++ $(MAC_INCLUDES) $(SOURCE) $(MAC_LIBS) -o $(OUTPUT) $(FLAGS) 

build_sanitized:
	clang++ $(LIBS) $(SOURCE) -o $(OUTPUT)_san $(FLAGS) -fsanitize=address
