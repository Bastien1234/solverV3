TARGET = ${prog}

build:
	g++ -std=c++17 *.cpp -Wall -o prog

buildfast:
	g++ -std=c++17 *.cpp -O3 -Wall -o prog