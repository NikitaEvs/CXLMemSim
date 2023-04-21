//
// Created by victoryang00 on 4/17/23.
//

#ifndef CXL_MEM_SIMULATOR_PT_H
#define CXL_MEM_SIMULATOR_PT_H
#include "helper.h"
#include "perf.h"
#include <intel-pt.h>
#include <cstdint>

struct PerfConfig;
class PT {
public:
    uint32_t unc_idx;
    PerfInfo *perf;
    PT(const uint32_t unc_idx, PerfConfig *perf_config);
    ~PT() = default;

    int decode_instruction(struct CBOElem *elem);
};

#endif // CXL_MEM_SIMULATOR_PT_H