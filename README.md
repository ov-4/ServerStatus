# ServerStatus

Server Status implemented by 100% C++

*NB: some features in web browser are coded in JavaScript currently, but will be done with WebAssembly in the future*



## Build

Dependencies
`libgrpc++-dev libprotobuf-dev protobuf-compiler-grpc make cmake g++ gcc`

## Build Web

Install emsdk
```bash
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
```

Active env
```bash
source ./emsdk_env.sh
```

Build
```
cd web/
mkdir build
cd build
emcmake cmake ..
make
```

## Clean
```bash
rm build/ web/build -rf
```