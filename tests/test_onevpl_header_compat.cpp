#include "src/backend/onevpl/onevpl_header_compat.h"
#include <cassert>

// Exercise both ABI shapes without requiring either oneVPL installation.
struct LegacyMbqp { uint32_t reserved[10]{}; };
struct ModernMbqp { uint32_t Pitch = 0; };

int main() {
    LegacyMbqp old;
    ModernMbqp current;
    assert(mfx50rt::onevpl::setMbqpPitch(old, 80, 80));
    assert(!mfx50rt::onevpl::setMbqpPitch(old, 96, 80));
    for (auto value : old.reserved) assert(value == 0);
    assert(mfx50rt::onevpl::setMbqpPitch(current, 96, 80));
    assert(current.Pitch == 96);
    assert(!mfx50rt::onevpl::setMbqpPitch(current, 79, 80));
    assert(current.Pitch == 96);
    assert(!mfx50rt::onevpl::setMbqpPitch(old, 0, 0));
}
