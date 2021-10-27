all: example.so a.out

CXXFLAGS= -g -I./src/cpp_ppc -I./src/pycpp_ppcyp/ -std=c++14 -I /usr/include/python3.8/ /usr/lib/x86_64-linux-gnu/libpython3.8.so -D"DEBUG=0"

SRC=src/cpp_ppc/Error.cpp src/cpp_ppc/Reflection.cpp src/pycpp_ppcyp/PyCppType.cpp src/pycpp_ppcyp/PyCppReflection.cpp
example.so: main.cpp $(SRC) src/pycpp_ppcyp/PyCppCast.h
	g++ main.cpp $(SRC) -o $@ -shared -fPIC $(CXXFLAGS)


test.so: test/testlib.cpp $(SRC) src/pycpp_ppcyp/PyCppCast.h
	g++ test/testlib.cpp $(SRC) -o $@ -shared -fPIC $(CXXFLAGS)

a.out: $(SRC)
	g++ $(SRC) -o $@ $(CXXFLAGS)

#a.out: main.cpp
#	g++ $< -o $@ $(CXXFLAGS)

clean:
	rm -f example.so a.out

test:
	./a.out
	python test.py

