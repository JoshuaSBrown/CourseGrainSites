all: testCluster

testCluster: testCluster.cpp libCluster.o
	g++ -Wall -Wextra -std=c++11 -o testCluster libCluster.o testCluster.cpp

libCluster.o: libCluster.cpp
	g++ -Wall -Wextra -std=c++11 -c libCluster.cpp

.PHONY: clean
clean:
	$(RM) *.o
