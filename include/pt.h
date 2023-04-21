//
// Created by victoryang00 on 4/17/23.
//

#ifndef CXL_MEM_SIMULATOR_PT_H
#define CXL_MEM_SIMULATOR_PT_H
#include "helper.h"
#include "perf.h"
#include "pebs.h"
#include <intel-pt.h>
#include <cstdint>

struct PerfConfig;// Use PT to hook on other process to make back invalidation
class PT {
public:
    uint32_t unc_idx;
    PerfInfo *perf;
    PT(const pid_t pid);
    ~PT() = default;

    int decode_instruction();
};

#endif // CXL_MEM_SIMULATOR_PT_H