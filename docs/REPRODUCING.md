# 构建与验证 / Build & Verification

## CPU-only（Linux / macOS）

```bash
cmake -S . -B build-cpu -DMFX50RT_BUILD_HARDWARE=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-cpu --parallel
ctest --test-dir build-cpu --output-on-failure
./build-cpu/policy_decision_demo
```

该配置构建真实策略库和独立算法/队列单测，不创建模拟视频编码器。需要 CMake 3.16+ 和 C/C++17 编译器。所有 `test_` 目标都强制取消 `NDEBUG` 并预包含断言保护头，因此 Release 构建仍执行 `assert` 内的 API 调用。

This profile tests policy and algorithm code; it neither encodes video nor measures hardware throughput. Release assertions are explicitly enabled and guarded at compile time.

## Linux oneVPL SDK

Ubuntu 24.04 示例依赖：

```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake libvpl-dev libva-dev
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

完整 CMake 默认仍为 `MFX50RT_BUILD_HARDWARE=ON`，保留原接口与硬件路径。无 GPU 的 Linux CI 只编译该 SDK；实际运行另需匹配的 Intel 驱动、oneVPL GPU runtime、B570 和可读写 DRM render node。

B570 可能对应不同设备节点，应先核对本机设备，再设置脚本中的 `DEVICE`。历史示例采用 `/dev/dri/renderD129`，该路径不是所有机器通用。

运行入口：

- [单文件脚本](../examples/b570_fastpath/run_single_mp4.sh)
- [输入清单与 45 路示例](../examples/b570_fastpath/run_45_from_manifest.sh)
- [昼夜策略示例](../examples/b570_fastpath/run_45_daynight_from_mp4_dir.sh)
- [API 与 SDK 使用说明](B570_API_REFERENCE.md)

MP4 解封装/封装脚本还需要 FFmpeg 命令。RTSP/UDP C++ 示例默认关闭；启用 `MFX50RT_BUILD_FFMPEG_DEMOS=ON` 时另需 FFmpeg 开发库。

## 验证层级

| 检查 | 能证明 | 不能证明 |
|---|---|---|
| CPU Debug / Release 单测 | 策略、边界、队列语义与断言执行 | B570 编解码功能或性能 |
| Linux SDK 编译 | 当前 Linux 依赖下源码可编译链接 | GPU 已正确选中、运行控制生效 |
| B570 实际转码 | 对应环境与输入下的媒体输出、排空和控制 | 未测试输入或长时间运行保证 |
| 多路与质量评测 | 固定分辨率/帧率/配置下的吞吐和质量 | 任意码流上均满足目标 |

性能复测应按 [测量协议](BENCHMARKS.md) 记录条件；CPU CI 与无 GPU 编译不会覆盖这些结论。

## oneVPL 头文件兼容

Ubuntu 的稳定 oneVPL 头文件与较新 API 不完全一致。运行时和探针通过字段检测设置 `mfxExtMBQP::Pitch`：旧头文件仅接受连续排列的 QP 图，不能表达的非连续 stride 会明确拒绝，不会静默丢弃。可选错误枚举通过 CMake 编译探测决定是否加入诊断。此兼容处理不等于旧驱动支持新硬件功能。

设计依据：[Intel MBQP 数据结构](https://intel.github.io/libvpl/latest/API_ref/VPL_structs_encode.html) 与 [API 字段演进记录](https://intel.github.io/libvpl/latest/Experimental.html)。实际是否支持 MBQP 仍由运行探测确认。
