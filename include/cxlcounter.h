//
// Created by victoryang00 on 1/12/23.
//

#ifndef CXL_MEM_SIMULATOR_CXLCOUNTER_H
#define CXL_MEM_SIMULATOR_CXLCOUNTER_H

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include <tuple>

class CXLSwitchEvent {
public:
    uint64_t load = 0;
    uint64_t store = 0;
    uint64_t conflict = 0;

    CXLSwitchEvent() = default;
    CXLSwitchEvent(const CXLSwitchEvent &other) = default;
    void inc_load();
    void inc_store();
    void inc_conflict();
};
class CXLMemExpanderEvent {
public:
    uint64_t load = 0;
    uint64_t store = 0;
    uint64_t migrate = 0;
    uint64_t hit_old = 0;

    CXLMemExpanderEvent() = default;
    CXLMemExpanderEvent(const CXLMemExpanderEvent &other) = default;
    void inc_load();
    void inc_store();
    void inc_migrate(); // happens access to after migration
    void inc_hit_old(); // happens during access to the prev one if not migration or backinv
};
class CXLCounter {
public:
    uint64_t local = 0;
    uint64_t remote = 0;
    uint64_t hitm = 0;

    CXLCounter() = default;
    CXLCounter(const CXLCounter &other) = default;
    void inc_local();
    void inc_remote();
    void inc_hitm(); // happens when access to a local modified cacheline.
};

#endif // CXL_MEM_SIMULATOR_CXLCOUNTER_H
