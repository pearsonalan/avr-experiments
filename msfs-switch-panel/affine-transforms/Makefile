all: test-affine

test-affine: test-affine.cc affine.cc
	g++ -O2 -std=c++17 -D AFFINE_IOSTREAMS=1 -I ../../fixed-point -lm -o test-affine test-affine.cc affine.cc