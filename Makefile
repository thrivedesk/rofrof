override CXXFLAGS += -static-libasan -g -pthread -lpthread -flto -O0 -Wconversion -std=c++2a -IuWebSockets/src -IuWebSockets/uSockets/src -DUWS_WITH_PROXY
override LDFLAGS += uWebSockets/uSockets/*.o -lz -lssl -lcrypto -luv -ljsoncpp

.PHONY: clean build

build:
	WITH_OPENSSL=1 $(MAKE) -C uWebSockets/uSockets; \
	mkdir -p build; \
	$(CXX) $(CXXFLAGS) src/main.cpp -o build/rofrof $(LDFLAGS);

clean:
	rm -rf build;

all: clean build

run:
	clean
	build
	build/rofrof
