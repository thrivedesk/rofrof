override CXXFLAGS += -pthread -lpthread -flto -Wconversion -std=c++2a -IuWebSockets/src -IuWebSockets/uSockets/src -DUWS_WITH_PROXY
override LDFLAGS += uWebSockets/uSockets/*.o -lz -lssl -lcrypto -luv -ljsoncpp

.PHONY: clean build

compile_debug:
	$(CXX) -static-libasan -g -O0 $(CXXFLAGS) src/main.cpp -o build/rofrof $(LDFLAGS);

compile_release:
	$(CXX) -fsanitize=address -O3 $(CXXFLAGS) src/main.cpp -o build/rofrof $(LDFLAGS);

build_socket:
	WITH_OPENSSL=1 $(MAKE) -C uWebSockets/uSockets;

make_dir:
	mkdir -p build;

debug: build_socket make_dir compile_debug
build: build_socket make_dir compile_release

clean:
	rm -rf build;

all: clean build

run:
	clean
	build
	build/rofrof
