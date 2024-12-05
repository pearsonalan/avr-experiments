all: obj obj/test obj/print_sin obj/print_cos obj/sin_lookup obj/sin_lookup_fp obj/cos_lookup_fp

obj:
	mkdir obj

obj/test: FixedPointTest.cc
	gcc -std=c++17 -O2 -o obj/test -I . FixedPointTest.cc

obj/print_sin: print_sin.cc
	gcc -std=c++17 -O2 -o obj/print_sin -lm print_sin.cc

obj/print_cos: print_cos.cc
	gcc -std=c++17 -O2 -o obj/print_cos -lm print_cos.cc

obj/sin_lookup: sin_lookup.cc
	gcc -std=c++17 -Wno-psabi -O2 -o obj/sin_lookup -lm sin_lookup.cc

obj/sin_lookup_fp: sin_lookup_fp.cc
	gcc -std=c++17 -Wno-psabi -O2 -o obj/sin_lookup_fp -lm sin_lookup_fp.cc

obj/cos_lookup_fp: cos_lookup_fp.cc
	gcc -std=c++17 -Wno-psabi -O2 -o obj/cos_lookup_fp -lm cos_lookup_fp.cc

