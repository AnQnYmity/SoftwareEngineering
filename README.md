# 记账本系统 - C++ 实现

根据UML设计文档，这是一个完整的记账本系统C++实现。

## 项目结构

```
proj1/
├── include/                    # 头文件目录
│   ├── models/                # 数据模型
│   │   ├── Transaction.h      # 交易类
│   │   ├── Category.h         # 分类类
│   │   ├── Account.h          # 账户类
│   │   └── Settings.h         # 设置类
│   ├── storage/               # 存储层
│   │   ├── IStorage.h         # 存储接口
│   │   ├── FileStorage.h      # 文件存储实现
│   │   └── TransactionRepository.h  # 交易仓库
│   ├── services/              # 业务逻辑层
│   │   ├── StatisticsService.h      # 统计服务
│   │   ├── NotificationService.h    # 通知服务
│   │   └── ImportExportService.h    # 导入导出服务
│   └── controller/            # 控制层
│       └── TransactionController.h  # 交易控制器
└── src/                       # 源文件目录
    ├── main.cpp              # 主程序
    ├── FileStorage.cpp
    ├── TransactionRepository.cpp
    ├── StatisticsService.cpp
    ├── NotificationService.cpp
    ├── ImportExportService.cpp
    └── TransactionController.cpp

```

## 核心功能

### 1. 数据模型
- **Transaction**: 交易记录，包含金额、类型、日期、分类、备注等
- **Category**: 交易分类
- **Account**: 账户信息
- **Settings**: 系统设置（预算、提醒阈值等）

### 2. 存储层 (Storage Layer)
- **IStorage**: 存储接口，定义存储操作规范
- **FileStorage**: 文件存储实现，使用JSON格式存储数据
- **TransactionRepository**: 交易仓库，提供CRUD操作

### 3. 业务逻辑层 (Services Layer)
- **StatisticsService**: 
  - 计算月度总计
  - 分类统计分析
  - 资产趋势分析
  
- **NotificationService**: 
  - 检查预算阈值
  - 生成提醒通知
  - 事件监听机制

- **ImportExportService**:
  - JSON导入导出
  - CSV导入导出

### 4. 控制层 (Controller Layer)
- **TransactionController**: 
  - 统一的业务接口
  - 协调各个服务组件

## 编译和运行

### 前置需求
- C++17编译器
- nlohmann/json库

### 编译步骤

1. 进入项目目录
```bash
cd proj1
```

2. 编译
```bash
g++ -std=c++17 -I./include src/*.cpp -o accounting_system.exe 2>&1
```

### 运行
```bash
./accounting_system    # Linux/macOS
.\accounting_system.exe  # Windows
```

## 主要特性

✓ **交易管理**: 添加、编辑、删除、查询交易
✓ **数据统计**: 月度统计、分类分析、资产趋势
✓ **预算提醒**: 支持设置月度预算和阈值提醒
✓ **数据导入导出**: JSON和CSV格式
✓ **数据持久化**: 基于文件系统的存储
✓ **事件通知**: 异步通知机制

## 设计模式应用

- **Repository Pattern**: TransactionRepository 提供数据访问抽象
- **Service Layer Pattern**: 业务逻辑与数据访问分离
- **Dependency Injection**: 通过构造函数注入依赖
- **Observer Pattern**: NotificationService 的监听机制
- **Factory Pattern**: 数据生成和转换
- **Strategy Pattern**: 多种导出策略

## 时序流程

### 新增交易流程
1. 用户在UI输入交易数据
2. Controller 接收请求，调用 Service 验证
3. Service 转换为 Transaction 实体，调用 Repository 保存
4. Repository 调用 IStorage 持久化
5. Service 触发 StatisticsService 更新统计
6. Service 触发 NotificationService 检查阈值
7. 结果返回给 Controller，展示给用户

## 数据存储

所有数据存储在 `data/` 目录下的JSON文件中：
- `transactions.json`: 所有交易记录
- 其他配置文件（可扩展）

## 扩展点

1. **数据库支持**: 可以实现 `IStorage` 接口，支持SQLite/MySQL
2. **UI界面**: 可以添加Qt或者Web前端
3. **图表展示**: 集成图表库进行数据可视化
4. **数据加密**: 添加加密功能保护敏感数据
5. **多用户支持**: 添加用户认证和隔离

## 系统架构图

```
┌─────────────────┐
│   Main Entry    │
└────────┬────────┘
         │
┌────────▼───────────────────────────────────┐
│     TransactionController                   │
│  (统一入口，协调各个服务)                    │
└────────┬────────────┬──────────┬───────────┘
         │            │          │
    ┌────▼──┐    ┌───▼────┐    └─►NotificationService
    │Stats  │    │Import  │
    │Service│    │Export  │
    └────┬──┘    │Service │
         │       └────┬────┘
         └────┬────────┤
              │        │
         ┌────▼────────▼──────────────┐
         │ TransactionRepository       │
         │ (数据访问层)                │
         └────┬─────────────────────┘
              │
         ┌────▼──────────────┐
         │  IStorage          │
         │  (存储接口)        │
         └────┬──────────────┘
              │
         ┌────▼──────────────┐
         │ FileStorage        │
         │ (文件存储实现)     │
         └────────────────────┘
```

## 许可证

此项目为学习和演示用途。




