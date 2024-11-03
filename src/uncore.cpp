//
// Created by victoryang00 on 1/12/23.
//

#include "uncore.h"
extern Helper helper;
Uncore::Uncore(const uint32_t unc_idx, PerfConfig *perf_config) {
    unsigned long value;
    int r;
    char path[64], buf[32];
    memset(path, 0, sizeof(path));
    snprintf(path, sizeof(path) - 1, perf_config->path_format_cha_type.c_str(), unc_idx);

    fd = open(path, O_RDONLY);
    if (fd < 0) {
        LOG(ERROR) << fmt::format("open {} failed", path);
        throw std::runtime_error("open");
    }

    memset(buf, 0, sizeof(buf));
    r = read(fd, buf, sizeof(buf) - 1);
    if (r < 0) {
        LOG(ERROR) << fmt::format("read {} failed", fd);
        close(fd);
        throw std::runtime_error("read");
    }
    close(fd);

    value = strtoul(buf, nullptr, 10);
    if (value == ULONG_MAX) {
        LOG(ERROR) << fmt::format("strtoul {} failed", fd);
        throw std::runtime_error("strtoul");
    }

    for (size_t k = 0; k < this->perf.size(); ++k) {
        auto& v = this->perf[k];
        v = init_uncore_perf(-1, (int)unc_idx, std::get<1>(perf_config->cha[k]), std::get<2>(perf_config->cha[k]),
                             value);
    }
}

int Uncore::read_cha_elems(struct CHAElem *elem) {
    ssize_t r;
    for (size_t idx = 0; idx < this->perf.size(); ++idx) {
        auto& value = this->perf[idx];
        r = value->read_pmu(&elem->cha[idx]);
        if (r < 0) {
            LOG(ERROR) << fmt::format("read cha_elems[{}] failed.\n", std::get<0>(helper.perf_conf.cha[idx]));
            return r;
        }
        LOG(DEBUG) << fmt::format("read cha_elems[{}]:{}\n", std::get<0>(helper.perf_conf.cha[idx]), elem->cha[idx]);
    }

    return 0;
}
