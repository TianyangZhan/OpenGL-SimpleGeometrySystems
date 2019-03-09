#
# Tianyang Zhan
# SID: 913739374
#

graphics: main.o parser.o
	g++ -o graphics main.o parser.o -lglut -lGL

main.o: main.cpp parser.h
	g++ -Wall -c main.cpp

parser.o: parser.cpp parser.h
	g++ -Wall -c parser.cpp

clean:
	rm *.o graphics
