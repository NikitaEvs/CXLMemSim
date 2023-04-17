//
// Created by victoryang00 on 4/17/23.
//

#ifndef CXL_MEM_SIMULATOR_MIGRATION_H
#define CXL_MEM_SIMULATOR_MIGRATION_H
#include "helper.h"
#include "perf.h"
#include <cstdint>

struct PerfConfig;
class Migration {
public:
    uint32_t unc_idx;
    PerfInfo *perf;
    Migration(const uint32_t unc_idx, PerfConfig *perf_config);
    ~Migration() = default;

    int read_cbo_elems(struct CBOElem *elem);
};

#endif // CXL_MEM_SIMULATOR_MIGRATION_H