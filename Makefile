override CXXFLAGS += -pthread -lpthread -flto -O3 -Wconversion -std=c++2a -IuWebSockets/src -IuWebSockets/uSockets/src -DUWS_WITH_PROXY -fsanitize=address -g
override LDFLAGS += -lz -lssl -lcrypto -luv -ljsoncpp

.PHONY: clean build

build:
	WITH_OPENSSL=1 $(MAKE) -C uWebSockets/uSockets; \
	mkdir -p build; \
	$(CXX) $(CXXFLAGS) src/main.cpp -o build/rofrof $(LDFLAGS);

clean:
	rm -rf build;

all: clean build
