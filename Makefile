SHELL := /bin/bash

all:
	cd ldns; \
	./configure --prefix=/home/isa2015/ldns/ --disable-dane-verify; \
	make; \
	make install; \
	cd ..; \
	g++ -std=c++11 -I/home/isa2015/ldns/include/ src/*.cpp -L/home/isa2015/ldns/lib -lldns -Wl,-rpath=/home/isa2015/ldns/lib -o ./roughDNS
