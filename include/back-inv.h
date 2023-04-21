//
// Created by victoryang00 on 4/20/23.
//

#ifndef CXL_MEM_SIMULATOR_BACKINV_H
#define CXL_MEM_SIMULATOR_BACKINV_H
#include "pt.h"
#include "perf.h"
#include <cstdint>

struct PerfConfig;
class BackInv {
public:
    uint32_t unc_idx;
    PerfInfo *perf;
    BackInv(const uint32_t unc_idx, PerfConfig *perf_config);
    ~BackInv() = default;

    int decode_instruction(struct CBOElem *elem);
};

#endif // CXL_MEM_SIMULATOR_BACKINV_H