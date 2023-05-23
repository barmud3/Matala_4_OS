CXX = clang-14
CXXFLAGS = -Wall -Wextra -std=c++17

.PHONY: all clean

all: react_server

react_server: Server.cpp libst_reactor.so
	$(CXX) $(CXXFLAGS) -o react_server Server.cpp -L. -lst_reactor -lstdc++ -ldl -pthread

libst_reactor.so: st_reactor.cpp st_reactor.hpp
	$(CXX) $(CXXFLAGS) -fPIC -shared -o libst_reactor.so st_reactor.cpp

clean:
	rm -f react_server libst_reactor.so
