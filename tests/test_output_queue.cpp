#include "src/core/mfx50_output_queue.h"

#include <cassert>
#include <cstring>

int main() {
    mfx50rt::OutputQueue queue;
    MFX50RT_EncodedPacket out{};
    assert(queue.poll(nullptr) == MFX50_ERR_INVALID_ARG);
    assert(queue.poll(&out) == MFX50_ERR_NO_OUTPUT);

    mfx50rt::OutputPacket first;
    first.streamId = 7;
    first.data = {1, 2, 3};
    first.pts = 90;
    first.dts = 80;
    first.isKeyframe = 1;
    queue.push(first);
    first.data[0] = 99; // Enqueue owns a copy; producer buffer can be reused.
    mfx50rt::OutputPacket second;
    second.streamId = 8;
    second.data = {4, 5};
    queue.push(second);

    uint8_t bytes[3]{};
    out.data = bytes;
    out.capacity = 2;
    assert(queue.poll(&out) == MFX50_ERR_BUFFER_TOO_SMALL);
    assert(out.size == 3);
    assert(queue.size() == 2); // A sizing retry must not discard a packet.
    out.capacity = sizeof(bytes);
    assert(queue.poll(&out) == MFX50_OK);
    const uint8_t expected[] = {1, 2, 3};
    assert(std::memcmp(bytes, expected, 3) == 0);
    assert(out.stream_id == 7 && out.pts == 90 && out.dts == 80);
    assert(out.is_keyframe == 1 && queue.size() == 1);
    auto remaining = queue.drainAll();
    assert(remaining.size() == 1 && remaining.front().streamId == 8);
    assert(queue.empty());
    assert(queue.poll(&out) == MFX50_ERR_NO_OUTPUT);
}
