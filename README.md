# RofRof


### Development

Standard: C++2a

### Dependencies
* uNetWorking/uWebSockets#v18.15.0
* jsoncpp

### Prepare

```bash
git submodule update --init --recursive
```

### Build

```bash
make build
or
make

g++ -flto -O3 -Wconversion -std=c++2a -lpthread -pthread -IuWebSockets/src -IuWebSockets/uSockets/src src/main.cpp -o main uWebSockets/uSockets/*.o -lz -lssl -lcrypto -luv -ljsoncpp && ./main
```

### Run
```
./build/rofrof
```
