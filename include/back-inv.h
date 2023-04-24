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
    PTElem *pt;
    BackInv(const uint32_t unc_idx, PTElem *pt);
    ~BackInv() = default;
};

#endif // CXL_MEM_SIMULATOR_BACKINV_H