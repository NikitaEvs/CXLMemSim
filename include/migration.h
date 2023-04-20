//
// Created by victoryang00 on 4/17/23.
//

#ifndef CXL_MEM_SIMULATOR_MIGRATION_H
#define CXL_MEM_SIMULATOR_MIGRATION_H
#include "helper.h"
#include "cxlcontroller.h"
#include <cstdint>
// transparent slow tier promote to higher tier
// support both page level, that do the reverse mapping of the process. and the cacheline level
class Migration : Policy {
public:
    std::map<uint64_t,int> pending_graph;
    Migration();
    ~Migration() = default;

    int compute_once(CXLController *) override;
};

#endif // CXL_MEM_SIMULATOR_MIGRATION_H