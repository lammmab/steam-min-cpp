# Steam Minimal C++

## About

Minimal Steam connections written in C++.This includes logic for:
- Connecting to a Steam CM server
- Logging in as an anonymous user
- Getting product details given a product ID

It is built to support future contributions extending the client to support other Steam server features.

## Requirements

- libcurl
- protobuf

## Building

1. Clone the repo
```bash
git clone https://git.petar.cc/gee.wzz/steam-min-cpp.git
```

2. CD into the repo
```bash
cd steam-min-cpp
```

3. Make a build folder
```bash
mkdir build
```

4. CD into the build folder
```bash
cd build
```

5. Configure the build system
```bash
cmake ..
```

6. Build the project
```bash
make
```