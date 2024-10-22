# RofRof


### Development

Standard: C++2a

### Dependencies
* uNetWorking/uWebSockets#v18.15.0
* libjsoncpp-dev
* libabsl
    * http://archive.ubuntu.com/ubuntu/pool/universe/a/abseil/libabsl20200225_0~20200225.2-3_amd64.deb
    * http://archive.ubuntu.com/ubuntu/pool/universe/a/abseil/libabsl-dev_0~20200225.2-3_amd64.deb
* libssl-dev
* libboost1.71-dev
* zlib1g-dev
* libuv1-dev

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

### Progress

- [x] App support
- [x] Client message 
- [x] Private Channel 
- [x] Presence Channel 
- [x] HTTP Channels API 
- [x] HTTP Channel API 
- [x] HTTP User API 
- [x] HTTP Trigger API 
- [x] Signature verification 
- [x] Concurrent user limit per app  
- [ ] Hostname check  
- [ ] Path check  
- [x] Thread safe
- [x] Multithreading
- [ ] Cluster support
- [ ] Webhook support
- [ ] Write tests
- [ ] Write benchmark
- [ ] Optimize code
- [ ] Optimize memory and computation

### TODO:
- [ ] Random number generator has bug
- [ ] There's a memory leak somewhere in channel construction