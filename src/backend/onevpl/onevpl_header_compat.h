#pragma once

#include <cstdint>
#include <type_traits>
#include <utility>

namespace mfx50rt::onevpl {

template <typename T, typename = void>
struct HasMbqpPitch : std::false_type {};

template <typename T>
struct HasMbqpPitch<T, std::void_t<decltype(std::declval<T&>().Pitch)>> : std::true_type {};

// Older stable oneVPL headers only describe packed raster-order QP maps.
// Never discard a nontrivial stride when compiling against those headers.
template <typename Mbqp>
bool setMbqpPitch(Mbqp& buffer, uint32_t pitch, uint32_t columns) {
    if (columns == 0 || pitch < columns) return false;
    if constexpr (HasMbqpPitch<Mbqp>::value) {
        buffer.Pitch = pitch;
        return true;
    } else {
        (void)buffer;
        return pitch == columns;
    }
}

} // namespace mfx50rt::onevpl
