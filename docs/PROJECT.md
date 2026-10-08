# 设计与个人贡献 / Design & Ownership

作者 **ipao666** 独立完成项目设计、代码实现、集成调试、验证与文档。Intel oneVPL、VAAPI、FFmpeg 及硬件编解码器是第三方组件，本项目没有从零实现 H.264/HEVC 编解码算法。

Independent personal project by ipao666. Contributions cover the SDK, scheduling, control policies, integration and verification; vendor codecs and third-party libraries retain their original authorship.

## 工程问题与代码证据

| 问题 | 实现与阅读入口 |
|---|---|
| 多路任务如何在硬件异步执行下推进 | `mfx50_realtime.cpp` 中 `trySubmitOneDecode`、`trySyncOneDecode`、`trySubmitOneEncode`、`trySyncOneEncode` 和 `processRoutePipelineSlice` |
| 如何处理 MORE_DATA 和尾帧 | 同文件 `drainPipeline`、`drainEncoder`、`MFX50RT_Flush`；[flush 修复记录](MFX50_REALTIME_FLUSH_FIX_20260609.md) |
| surface 的 backing 与 crop 不一致 | [尺寸检查修复](MFX50_ENCODER_RESOLUTION_BUFFER_FIX_20260608.md)、`tests/test_split_encoder_surface_shape.cpp` |
| 调用方输出缓冲太小如何重试 | `src/core/mfx50_output_queue.cpp`；新测试确认 BUFFER_TOO_SMALL 不弹出队首包，并检查复制所有权和 PTS/DTS 保留 |
| 策略选中与实际生效如何区分 | `src/backend/onevpl/` 的能力探测、适配与 decision trace；[后端说明](BACKEND_GUIDE.md) |
| 如何平衡细节保护和码率 | `src/algo/hybridtsrq/` 中时空 QP、质量保护与静态复用门控；[限制记录](B570_CURRENT_LIMITS.md) |

## 技术取舍

**减少不必要的数据搬运。** 多路全分辨率 NV12 回传会增加带宽和 CPU 压力。设计以硬件解码/编码及 surface 生命周期为中心，按受控频率做分析；不能仅凭接口存在就声称整条链路绝对零拷贝。

**处理状态，而不是只处理一次函数返回。** 硬件忙、需要更多输入、编码重排和异步完成具有不同语义。排空时必须把解码侧、待编码任务和编码缓存分别处理。

**能力探测后回退。** 配置允许请求某策略，但实际控制可能回退为全局 QP。effective strategy 与 actual encode control 都需要记录，避免把配置意图当作执行证据。

**明确质量指标的失败场景。** 高 QP 对白天纹理丰富场景的 Y 通道伤害明显；降低 QP 保护细节会损失压缩率。项目保留失败记录，未声称所有约束同时满足。

## 简历表述参考 / Résumé wording

> 独立开发基于 Intel oneVPL 的 C/C++ 多路硬件转码 SDK，实现异步解码/编码流水线、surface 管理、输出队列与 flush/drain；设计 QP/ROI 质量策略、能力探测及诊断工具，并分析吞吐、压缩率和画质的权衡。

> Independently developed a C/C++ multi-stream transcoding SDK using Intel oneVPL, implementing asynchronous pipeline control, surface management, output queues and draining, with adaptive quality policies and diagnostics.

历史性能数字如用于简历，应同时写明测试规模与作者记录属性，并保留原始日志供面试核查。当前公开仓库不足以独立复现全部性能结论。

## 演示顺序

1. 使用 CPU-only Release 构建运行单测和 `policy_decision_demo`。
2. 展示策略 API 输入/输出，进入 QP 与质量保护实现。
3. 解释异步 pipeline 中任务的生命周期以及 flush 为什么必须分阶段处理。
4. 展示历史验证结果，解释最低路吞吐与全局吞吐、Y 通道与整体 SSIM 的区别。
