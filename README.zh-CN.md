[English](README.md) | [中文](README.zh-CN.md)

# WebCTP

WebCTP 是一个基于 WebSocket 的网关，将 CTP（上期技术综合交易平台，Comprehensive Transaction Platform）API 桥接到现代 Web 应用程序。它提供了 C++ 服务器实现和 TypeScript 客户端库，使开发者能够通过 WebSocket 连接与中国期货交易系统进行交互。

## 概述

WebCTP 封装了官方的 CTP API（ThostFtdcMdApi 和 ThostFtdcTraderApi），并通过 WebSocket 端点将其暴露出来，使其可以从 Web 浏览器、Node.js 应用程序和其他支持 WebSocket 的客户端访问。

## 特性

- **行情数据 WebSocket 端点** (`/market_data`): 实时行情数据流
- **交易 WebSocket 端点** (`/trade`): 报单录入、账户查询和交易操作
- **TypeScript 客户端库**: 功能完整的客户端 SDK，便于集成
- **高性能**: 基于 uWebSockets 实现高效的 WebSocket 处理
- **CTP API 兼容性**: 支持官方 CTP API 版本

## 架构

```
┌─────────────┐         WebSocket          ┌──────────────┐
│  TypeScript │ ◄─────────────────────────► │  C++ 服务器  │
│   客户端    │                             │   (WebCTP)   │
└─────────────┘                             └──────┬───────┘
                                                   │
                                                   │ CTP API
                                                   ▼
                                          ┌──────────────┐
                                          │  经纪公司 CTP  │
                                          │   服务器     │
                                          └──────────────┘
```

## 组件

### 服务器 (C++)

服务器使用 C++17 实现，使用以下技术：
- **uWebSockets**: 高性能 WebSocket 服务器
- **CTP API**: 官方 ThostFtdcMdApi 和 ThostFtdcTraderApi 库
- **JSON**: 用于消息序列化

### 客户端库 (TypeScript)

TypeScript 客户端库提供：
- `MarketData` 类: 用于订阅行情数据
- `Trade` 类: 用于交易操作和账户管理
- 基于事件驱动的 API，带有回调处理器

## 构建

### 前置要求

- CMake 3.10 或更高版本
- 支持 C++17 的编译器
- 已安装 CTP API 库：
  - `thostmduserapi_se` (行情数据 API)
  - `thosttraderapi_se` (交易 API)
- uWebSockets 库
- ICU 库

### 构建步骤

```bash
mkdir build
cd build
cmake ..
make
```

## 运行

使用默认设置启动服务器（localhost:8888）：

```bash
./WebCTP
```

或指定自定义地址和端口：

```bash
./WebCTP <地址> <端口>
```

## 使用

### 服务器端点

- **健康检查**: `GET /health`
- **行情数据**: `WS /market_data`
- **交易**: `WS /trade`

详细的文档和使用示例，请参阅：
- [WebSocket Server 接口](doc/websocket_server_cn.md)
- [TypeScript 库使用方法](doc/typescript_library_cn.md)

完整的文档索引请参见[文档目录](doc/README_CN.md)。

## 项目结构

```
WebCTP/
├── src/                    # C++ 服务器源代码
│   ├── main.cpp           # 服务器入口点
│   ├── MessageHandler.cpp # WebSocket 消息处理
│   ├── MarketData/        # 行情数据处理器
│   └── Trade/             # 交易处理器
├── lib/
│   └── ts/                # TypeScript 客户端库
│       └── src/
│           └── CTP/       # CTP 客户端类
└── CMakeLists.txt         # 构建配置
```

## 许可证

详情请参阅 [LICENSE](LICENSE) 文件。

