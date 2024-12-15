all: test-affine

test-affine: test-affine.cc ../../affine/affine.cc
	g++ -O2 -std=c++17 -D AFFINE_IOSTREAMS=1 -I ../../fixed-point/lib -I ../../affine -lm -o test-affine test-affine.cc ../../affine/affine.cc ../../fixed-point/lib/TrigLookup.cc
