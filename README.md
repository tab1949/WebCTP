[English](README.md) | [中文](README.zh-CN.md)

# WebCTP

WebCTP is a WebSocket-based gateway that bridges the CTP (Comprehensive Transaction Platform) API to modern web applications. It provides a C++ server implementation and a TypeScript client library, enabling developers to interact with Chinese futures trading systems through WebSocket connections.

## Overview

WebCTP wraps the official CTP API (ThostFtdcMdApi and ThostFtdcTraderApi) and exposes it via WebSocket endpoints, making it accessible from web browsers, Node.js applications, and other WebSocket-compatible clients.

## Features

- **Market Data WebSocket Endpoint** (`/market_data`): Real-time market data streaming
- **Trading WebSocket Endpoint** (`/trade`): Order insertion, account queries, and trading operations
- **TypeScript Client Library**: Full-featured client SDK for easy integration
- **High Performance**: Built on uWebSockets for efficient WebSocket handling
- **CTP API Compatibility**: Supports official CTP API versions

## Architecture

```
┌─────────────┐         WebSocket          ┌──────────────┐
│  TypeScript │ ◄─────────────────────────► │  C++ Server  │
│   Client    │                             │   (WebCTP)   │
└─────────────┘                             └──────┬───────┘
                                                   │
                                                   │ CTP API
                                                   ▼
                                          ┌──────────────┐
                                          │  CTP Broker  │
                                          │   Servers    │
                                          └──────────────┘
```

## Components

### Server (C++)

The server is implemented in C++17 and uses:
- **uWebSockets**: High-performance WebSocket server
- **CTP API**: Official ThostFtdcMdApi and ThostFtdcTraderApi libraries
- **JSON**: For message serialization

### Client Library (TypeScript)

The TypeScript client library provides:
- `MarketData` class: For subscribing to market data feeds
- `Trade` class: For trading operations and account queries
- Event-driven API with callback handlers

## Building

### Prerequisites

- CMake 3.10 or higher
- C++17 compatible compiler
- CTP API libraries installed:
  - `thostmduserapi_se` (Market Data API)
  - `thosttraderapi_se` (Trading API)
- uWebSockets library
- ICU library

### Build Steps

```bash
mkdir build
cd build
cmake ..
make
```

## Running

Start the server with default settings (localhost:8888):

```bash
./WebCTP
```

Or specify custom address and port:

```bash
./WebCTP <address> <port>
```

## Usage

### Server Endpoints

- **Health Check**: `GET /health`
- **Market Data**: `WS /market_data`
- **Trading**: `WS /trade`

For detailed documentation and usage examples, please refer to:
- [WebSocket Server API](doc/websocket_server_en.md)
- [TypeScript Library Usage](doc/typescript_library_en.md)

See also the [documentation index](doc/README_EN.md) for a complete overview.

## Project Structure

```
WebCTP/
├── src/                    # C++ server source code
│   ├── main.cpp           # Server entry point
│   ├── MessageHandler.cpp # WebSocket message handling
│   ├── MarketData/        # Market data handler
│   └── Trade/             # Trading handler
├── lib/
│   └── ts/                # TypeScript client library
│       └── src/
│           └── CTP/       # CTP client classes
└── CMakeLists.txt         # Build configuration
```

## License

See [LICENSE](LICENSE) file for details.

