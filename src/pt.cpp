//
// Created by victoryang00 on 4/17/23.
//

#include "pt.h"

struct tracing_state {
    bool failure_state;
    int intel_pt_type;
    int perf_fd;
    struct perf_event_mmap_page *header;
    char *base, *data, *aux;
    uint64_t base_mmap_size, aux_mmap_size;

    int pt_fd;
    int sb_fd;
};
#define or_ret(val) \
  do { \
    int __res = (val); \
    if (__res != 0) return __res; \
  } while (0)

static int write_all(int fd, const char *buf, size_t amt) {
    size_t written = 0;
    while (written < amt) {
        ssize_t res = write(fd, buf + written, amt - written);
        if (res < 0) return res;
        written += res;
    }
    return 0;
}
static int read_int_file(const char *path, int *result) {
    FILE* intel_pt_type_f = fopen(path, "r");
    if (intel_pt_type_f == NULL) return -1;
    int res = fscanf(intel_pt_type_f, "%d", result);
    fclose(intel_pt_type_f);
    return (res != 1) ? -1 : 0;
}

static int init_tracing_state(struct tracing_state *s) {
    if(read_int_file("/sys/bus/event_source/devices/intel_pt/type", &s->intel_pt_type))
        return -1;

    s->failure_state = false;
    s->perf_fd = -1;
    s->base = s->data = s->aux = NULL;
    s->base_mmap_size = s->aux_mmap_size = 0;

    return 0;
}
static int dump_perf_buffer(int fd, char *base,
                            uint64_t size, uint64_t head_, uint64_t tail_) {
     LOG(INFO)<<fmt::format("dumping {} {} {}\n", size, head_, tail_);
    char *end  = base + size;
    char *head = base + (head_ & (size - 1));
    char *tail = base + (tail_ & (size - 1));

    if (tail <= head) {
        or_ret(write_all(fd, tail, head - tail));
    } else {
        or_ret(write_all(fd, tail, end - tail));
        or_ret(write_all(fd, base, head - base));
    }

    return 0;
}

// See [lib/pmc/src/msr_stubs.c:187] for an explanation
#define rmb() asm volatile("" ::: "memory")


PT::PT(const pid_t pid) {
    struct perf_event_attr pe = {
        .type = PERF_TYPE_RAW,
        .size = sizeof(struct perf_event_attr),
        .config = 0x20d1, // mem_load_retired.l3_miss
        .sample_period = sample_period,
        .sample_type = PERF_SAMPLE_TID | PERF_SAMPLE_TIME | PERF_SAMPLE_ADDR | PERF_SAMPLE_READ | PERF_SAMPLE_PHYS_ADDR,
        .read_format = PERF_FORMAT_TOTAL_TIME_ENABLED,
        .disabled = 1, // Event is initially disabled
        .exclude_kernel = 0,
        .precise_ip = 1,
        .config1 = 3,
    }; // excluding events that happen in the kernel-space

    int cpu = -1; // measure on any cpu
    int group_fd = -1;
    unsigned long flags = 0;

    this->fd = perf_event_open(&pe, pid, cpu, group_fd, flags);
    if (this->fd == -1) {
        perror("perf_event_open");
        throw;
    }

    this->mplen = MMAP_SIZE;
    this->mp = (perf_event_mmap_page *)mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, this->fd, 0);

    if (this->mp == MAP_FAILED) {
        perror("mmap");
        throw;
    }

    this->start();
}
int PT::decode_instruction() {

}
