
ALL=rotatetest permtest jain

all: $(ALL)


%: %.cpp
	g++ -O4 -march=native -std=c++1y -Wall $< -o $*

clean:
	rm -f $(ALL)
