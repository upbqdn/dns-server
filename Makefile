all:
	echo Compiling $(CLION_EXE_DIR)/$@ ...
	g++ -std=c++11 src/*.cpp -ggdb3 -o $(CLION_EXE_DIR)/roughDNS

terminal:
	g++ src/*.cpp -o roughDNS