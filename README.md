# RofRof


### Development

Standard: C++2a

### Dependencies
* uNetWorking/uWebSockets#v18.15.0
* jsoncpp
* libabsl
    * http://archive.ubuntu.com/ubuntu/pool/universe/a/abseil/libabsl20200225_0~20200225.2-3_amd64.deb
    * http://archive.ubuntu.com/ubuntu/pool/universe/a/abseil/libabsl-dev_0~20200225.2-3_amd64.deb
* openssl
* libboost1.71-dev

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
