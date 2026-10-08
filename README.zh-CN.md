# Intel B570 多路硬件视频转码 SDK

### Intel B570 Multi-Stream Hardware Transcoding SDK

**面向 Intel B570 的 C/C++ 多路硬件转码与自适应质量策略。**
C/C++ multi-stream hardware transcoding and adaptive quality control for Intel B570.

[English](README.md) · [设计与个人贡献](docs/PROJECT.md) · [构建与验证](docs/REPRODUCING.md) · [性能证据](docs/BENCHMARKS.md) · [历史限制记录](docs/B570_CURRENT_LIMITS.md)

[![Build and tests](https://github.com/ipao666/intel-b570-transcoding-sdk/actions/workflows/ci.yml/badge.svg)](https://github.com/ipao666/intel-b570-transcoding-sdk/actions/workflows/ci.yml)

独立个人项目，作者 **[ipao666](https://github.com/ipao666)**。我负责 SDK 接口、oneVPL 集成、异步流水线、质量策略、诊断与测试。视频编解码底层由 Intel oneVPL / VAAPI 和硬件实现，本项目贡献在集成、调度、策略与工程验证。

代码中的 `MFX50` / `MFX50RT` 为既有 API 前缀，保留以兼容现有调用。

## 解决的问题

多路监控转码不仅需要足够的吞吐，还需要控制拷贝、排空尾帧、保留关键区域，并在压缩率与画质之间做可解释的选择。本项目提供可集成的 C 接口、C++ 实现和诊断工具，探索这些约束在 B570 上的实际边界。

```mermaid
flowchart LR
    A[编码输入与解封装] --> B[多路任务调度]
    B --> C[oneVPL 硬件解码]
    C --> D[Surface 与异步状态管理]
    D --> E[低频场景分析与 QP/ROI 决策]
    E --> F[HEVC 硬件编码]
    F --> G[输出队列与 Poll/回调]
    G --> H[调用方封装或推流]
```

能力探测与回退决定实际使用的编码控制。ROI/MBQP 支持与驱动、格式及运行路径有关，需检查有效配置和运行 trace；配置字段本身不证明控制已生效。

## 具体工程内容

| 模块 | 内容 | 入口 |
|---|---|---|
| 异步流水线 | 解码/编码提交、同步、顺序与 surface 生命周期 | [mfx50_realtime.cpp](mfx50_realtime.cpp) |
| 尾帧与错误处理 | MORE_DATA / DEVICE_BUSY、flush/drain 与输出排空 | [历史 flush 修复](docs/MFX50_REALTIME_FLUSH_FIX_20260609.md) |
| 质量策略 | 场景分析、时空 QP、质量保护、ROI 与静态复用门控 | [hybridtsrq](src/algo/hybridtsrq/) |
| 接口与诊断 | C API、能力查询、有效策略、决策 trace、JSON 配置 | [公开头文件](include/mfx50rt.h)、[后端说明](docs/BACKEND_GUIDE.md) |
| 可移植验证 | 无 GPU 策略演示、算法/队列单元测试、Release 断言保护 | [测试目录](tests/) |

## 无 GPU 快速体验

需要 CMake 3.16+ 和 C/C++17 编译器，Linux 或 macOS；无需 oneVPL、libva 或 B570。

```bash
git clone https://github.com/ipao666/intel-b570-transcoding-sdk.git
cd intel-b570-transcoding-sdk
cmake -S . -B build-cpu -DMFX50RT_BUILD_HARDWARE=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-cpu --parallel
ctest --test-dir build-cpu --output-on-failure
./build-cpu/policy_decision_demo
```

该演示把输入特征和 ROI 元数据转换成编码策略，不输出压缩视频。CPU 测试覆盖策略 API、QP 图、MBQP 数据适配、ROI 分析、质量保护、静态复用、预处理及输出队列。Release 测试显式保留断言，防止 API 调用被编译器删除。

## B570 硬件路径

需要 Linux x86_64、B570、可访问的 DRM render node、oneVPL 和 libva 开发库。完整命令、设备选择和验证边界见[构建指南](docs/REPRODUCING.md)。

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

## 已记录的性能与限制

以下为作者历史 **45 路 × 1000 帧**验证摘要，不是 CI 复跑结果。原始逐路 CSV、完整输入集及完整环境信息未随源码归档。

| 配置/指标 | 记录值 |
|---|---:|
| QPI/QPP/QPB | 41 / 43 / 51 |
| 最低路吞吐 | 32.410 FPS |
| 平均压缩率（历史报告口径） | 86.122% |
| 平均整体 SSIM | 0.895916 |
| 最低 Y 通道 SSIM | 0.699790 |

该配置达到记录中的吞吐目标，但没有同时达到约 90% 压缩和全部画质门槛。质量优先的白天配置记录了更好的最低 Y-SSIM，但平均压缩率降至 60.913%。这说明需要明确应用的质量与存储约束，不能承诺所有指标同时达标。[完整证据范围](docs/BENCHMARKS.md)

## 阅读顺序

- [设计与个人贡献](docs/PROJECT.md)：线程/状态、缓冲与策略回退的代码入口。
- [构建与验证](docs/REPRODUCING.md)：CPU 单测、Linux 编译与真实硬件验收的区别。
- [性能证据](docs/BENCHMARKS.md)：已知结果、缺失原始材料及重新测量协议。

使用 [MIT License](LICENSE)。厂商运行时、驱动与外部库遵循各自许可证。
