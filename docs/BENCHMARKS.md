# 性能证据与测量协议 / Benchmark Evidence

## 当前证据 / Available evidence

以下数值来自仓库原有 [B570_CURRENT_LIMITS.md](B570_CURRENT_LIMITS.md)，属于作者历史报告。没有在此次作品整理中重新测量。完整测试视频、逐路 CSV、设备/驱动快照和长稳日志尚未随源码归档。

These are historical author-reported measurements, not CI results or newly reproduced benchmarks. Missing raw inputs and environment metadata limit independent verification.

| 历史配置 | 已记录结果 | 边界 |
|---|---|---|
| 45 路 × 1000 帧，QPI/QPP/QPB=41/43/51 | 最低路 32.410 FPS；平均压缩 86.122%；平均整体 SSIM 0.895916；最低 Y-SSIM 0.699790 | 吞吐达标，压缩与画质未同时达标 |
| 质量优先，白天 36/38/44 | 平均压缩 60.913%；最低 Y-SSIM 0.812878；平均整体 SSIM 0.905795 | 质量保护带来明显压缩率代价 |
| 质量优先，夜间 47/49/51 | 平均压缩 97.064%；最低 Y-SSIM 0.864618；最低整体 SSIM 0.902120 | 特定夜间样本，不代表全部视频 |

没有完整分辨率和环境记录时，不应自行补写“45 路 1080p30”。文件数量与帧数也不能替代持续运行时长。

## 新一轮测量应保留什么

1. **环境**：Git commit、CPU/内存、GPU PCI ID、DRM 节点、驱动/oneVPL/内核版本。
2. **输入**：每路文件哈希、编码格式、分辨率、帧率、时长、输入字节数；说明不同路是否重复素材。
3. **配置**：QPI/QPP/QPB、GOP、B 帧、AsyncDepth、surface 路径、ROI/MBQP 请求与实际生效 trace。
4. **输出完整性**：解码帧数、编码帧数、flush 后帧数和时戳；区分实际丢帧、重排与抽帧。
5. **吞吐**：总 wall time、总输出帧数、每路 FPS 的最小值和分布；说明预处理/I/O 是否计入。
6. **压缩与质量**：输入/输出字节数，显式定义 `1 - output_bytes / input_bytes`；相同时间对齐和分辨率下的整体及 Y/U/V SSIM。
7. **稳定性**：不同于 1000 帧短测，另记录持续运行时长、内存、队列深度、错误、重连与输出完整性。

保存逐路 CSV 和可执行命令，再生成汇总。禁止把夜间压缩率、另一个配置的质量值和第三次运行的吞吐拼成一次“全部达标”。

## 已知工程范围

- C API、策略/算法和 oneVPL 集成在仓库内；厂商运行时与视频输入不在仓库内。
- CPU CI 检查算法和队列；Linux job 编译 SDK；均无 B570 运行验收。
- 策略接口存在不证明某硬件支持 ROI/MBQP；应检查运行探测与实际附加的控制。
- 暂无可公开核查的长期生产运行或服务可用性证据。
