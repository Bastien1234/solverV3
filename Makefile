TARGET = ${prog}

build:
	g++ -std=c++23 *.cpp -Wall -o prog

buildfast:
	g++ -std=c++23 *.cpp -O3 -Wall -o prog