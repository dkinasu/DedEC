all:
	g++ -o a.out main.cpp trace.cpp process.cpp cache.cpp storage.cpp memory.cpp rbtree.cpp
#   g++ -o test test.cpp memory.cpp storage.cpp rbtree.cpp trace.cpp process.cpp main.cpp

test: test
#   g++ -o test test.cpp memory.cpp storage.cpp rbtree.cpp trace.cpp process.cpp main.cpp
