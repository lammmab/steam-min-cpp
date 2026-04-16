# Steam Minimal C++

## About

A library for minimal Steam connections written in C++.
This includes logic for:
- Connecting to a Steam CM server
- Logging in as an anonymous user
- Getting product details given a product ID

It is built to support future contributions extending the client to support other Steam server features.
Please not that currently only TCP connections are supported.

## Requirements

- libcurl
- boost
- crypto++
- protobuf
- spdlog (OPTIONAL)

## Building

1. Clone the repo
```bash
git clone https://git.petar.cc/gee.wzz/steam-min-cpp.git
```

2. CD into the repo
```bash
cd steam-min-cpp
```

3. Configure the project 
```bash
cmake -B build -G Ninja

> [!TIP]
> In this example, Ninja Generator is used.

4. Build the project
```bash
cmake --build build 
```

## Documentation

Documentation can be found on the [read the docs page](https://steam-min-cpp.readthedocs.io/en/latest/)

## Examples

Examples can be found in [the examples folder](https://github.com/lammmab/steam-min-cpp/tree/main/examples)