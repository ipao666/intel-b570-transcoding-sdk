# Intel B570 多路硬件视频转码 SDK

### Intel B570 Multi-Stream Hardware Transcoding SDK

**C/C++ multi-stream hardware transcoding and adaptive quality control for Intel B570.**
面向 Intel B570 的 C/C++ 多路硬件转码与自适应质量策略。

[中文完整说明](README.zh-CN.md) · [Design & ownership](docs/PROJECT.md) · [Build & verification](docs/REPRODUCING.md) · [Benchmark evidence](docs/BENCHMARKS.md)

[![Build and tests](https://github.com/ipao666/intel-b570-transcoding-sdk/actions/workflows/ci.yml/badge.svg)](https://github.com/ipao666/intel-b570-transcoding-sdk/actions/workflows/ci.yml)

An independent personal project by **[ipao666](https://github.com/ipao666)**. My work covers SDK interfaces, oneVPL integration, asynchronous scheduling, quality policies, diagnostics and tests. The underlying codecs are implemented by Intel hardware and its runtime; this project builds the integration and control layer.

The existing `MFX50` / `MFX50RT` API prefixes are retained for interface compatibility.

## Problem and architecture

Multi-stream transcoding requires more than aggregate throughput: surfaces must remain valid across asynchronous operations, buffered frames must drain correctly, and bitrate reductions must be weighed against visual quality.

```text
Encoded input → per-stream scheduling → oneVPL decode → surface lifecycle
→ scene / QP / ROI policy → HEVC encode → output queue → caller muxing or streaming
```

- C API with C++17 implementation, versioned structures and configuration.
- Asynchronous decode/encode, ordered completion and flush/drain handling.
- Scene-aware QP decisions, quality guards, ROI/MBQP adapters and static-reuse gating.
- Capability probing, fallback explanations and decision traces.
- CPU-only policy demo and tests, plus Linux SDK compilation in CI.

ROI/MBQP availability depends on runtime and hardware. Check effective configuration and actual encode-control traces; a requested option is not proof that it was attached.

## Try it without a GPU

CMake 3.16+, a C/C++17 compiler, and Linux or macOS. No oneVPL or libva installation is needed for this profile.

```bash
git clone https://github.com/ipao666/intel-b570-transcoding-sdk.git
cd intel-b570-transcoding-sdk
cmake -S . -B build-cpu -DMFX50RT_BUILD_HARDWARE=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-cpu --parallel
ctest --test-dir build-cpu --output-on-failure
./build-cpu/policy_decision_demo
```

The demo produces policy decisions from input features/metadata; it does not transcode video. CPU tests cover the policy API, QP maps, MBQP data adaptation, ROI analysis, quality guards, static reuse, preprocessing and output queue retry semantics. Assertions stay enabled in Release tests.

## Hardware build

The full path requires Linux x86_64, Intel B570, a usable DRM render node, oneVPL and libva development libraries.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

See the [build guide](docs/REPRODUCING.md) for the hardware boundary and runtime examples.

## Recorded performance, with limitations

Historical author report: **45 streams × 1000 frames**, QPI/QPP/QPB = 41/43/51.

| Metric | Recorded value |
|---|---:|
| Minimum per-stream throughput | 32.410 FPS |
| Average compression reduction, report definition | 86.122% |
| Mean all-channel SSIM | 0.895916 |
| Minimum Y-channel SSIM | 0.699790 |

Throughput passed the reported target; compression and quality goals were not met simultaneously. A quality-first daytime profile reduced compression to 60.913%. Raw per-stream CSVs, the full input set and complete environment metadata are not included, so these are not independently reproduced benchmarks. CI does not execute B570 workloads. [Evidence and measurement protocol](docs/BENCHMARKS.md)

## Code tour

- [Realtime core](mfx50_realtime.cpp): asynchronous states, surfaces, submission, synchronization and drain.
- [Public API](include/mfx50rt.h) and [backend guide](docs/BACKEND_GUIDE.md): configuration, capabilities and fallback.
- [Quality policy](src/algo/hybridtsrq/): temporal/spatial QP and quality guard.
- [Output queue](src/core/mfx50_output_queue.cpp): ownership and retry behavior.
- [Design / personal contribution](docs/PROJECT.md): engineering choices and interview discussion points.

[MIT License](LICENSE); third-party runtimes and drivers retain their own licenses.
