CXXFLAGS = -rdynamic $(shell llvm-config --cxxflags) -g -O0 -std=c++0x

.PHONY: all clean

all: pass.so

%.so: %.o
	$(CXX) $(CXXFLAGS) -dylib -shared -fPIC  $^ -o $@

clean:
	rm -f *.o *.so *.bc *.ll *.out *.diff
