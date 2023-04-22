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
    bool failure_state;
    int intel_pt_type;
    int perf_fd;
    pid_t pid;
    struct perf_event_mmap_page *mp;
    char *base, *data, *aux;
    uint64_t base_mmap_size, aux_mmap_size;
    int pt_fd;
    int sb_fd;
    PerfInfo *perf;
    PT( pid_t pid,uint64_t);
    ~PT() = default;

    int decode_instruction(std::byte *byte_array);
    int read(CXLController *, struct PTElem *);
    int start();
    int stop();
};

#endif // CXL_MEM_SIMULATOR_PT_H