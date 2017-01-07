CXX = /usr/local/Cellar/llvm/3.9.0/bin/clang++

CXXFLAGS = -rdynamic $(shell /usr/local/Cellar/llvm/3.9.0/bin/llvm-config --cxxflags) -g -O0 -std=c++0x

all: MyModulePass.so MyFunctionPass.so MyBlockPass.so MyLoopPass.so MyRegionPass.so MySCCPass.so

%.so: %.o
	$(CXX) -v $(CXXFLAGS) -dylib -shared -fPIC  $^ -o $@ 

clean:
	rm -f *.o *~ *.so



