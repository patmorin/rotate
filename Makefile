
test: test.cpp
	g++ -O4 -std=c++1y -Wall test.cpp -o test

clean:
	rm -f test
