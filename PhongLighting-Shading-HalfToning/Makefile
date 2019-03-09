#
# Tianyang Zhan
# SID: 913739374
#

graphics: main.o parser.o
	g++ -std=c++11 -o graphics main.o parser.o -lglut -lGL

main.o: main.cpp parser.h
	g++ -Wall -std=c++11 -c main.cpp

parser.o: parser.cpp parser.h
	g++ -Wall -std=c++11 -c parser.cpp

clean:
	rm *.o graphics
