override CXXFLAGS += -pthread -lpthread -flto -O3 -Wconversion -std=c++2a -IuWebSockets/src -IuWebSockets/uSockets/src -DUWS_WITH_PROXY -fsanitize=address -g
override LDFLAGS += uWebSockets/uSockets/*.o -lz -lssl -lcrypto -luv  -lasan

.PHONY: clean build

build:
	$(MAKE) -C uWebSockets/uSockets; \
	mkdir -p build; \
	$(CXX) $(CXXFLAGS) main.cpp -o build/rofrof $(LDFLAGS);

clean:
	rm -rf build;
