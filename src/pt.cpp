//
// Created by victoryang00 on 4/17/23.
//

#include "pt.h"

static int read_int_file(const char *path, int *result) {
    FILE *intel_pt_type_f = fopen(path, "r");
    if (intel_pt_type_f == NULL)
        return -1;
    int res = fscanf(intel_pt_type_f, "%d", result);
    fclose(intel_pt_type_f);
    return (res != 1) ? -1 : 0;
}

PT::PT(pid_t pid, uint64_t sample_period) {
    if (read_int_file("/sys/bus/event_source/devices/intel_pt/type", &this->intel_pt_type))
        throw;

    this->failure_state = false;
    this->perf_fd = -1;
    this->base = this->data = this->aux = NULL;
    this->base_mmap_size = this->aux_mmap_size = 0;
    struct perf_event_attr pe = {
        .type = PERF_TYPE_HARDWARE,
        .size = sizeof(struct perf_event_attr),
        .config = PERF_COUNT_HW_INSTRUCTIONS,
        .sample_period = sample_period,
        .read_format = PERF_FORMAT_TOTAL_TIME_ENABLED,
        .disabled = 1, // Event is initially disabled
        .exclude_kernel = 0,
        .precise_ip = 1,
        .config1 = 3,
    }; // excluding events that happen in the kernel-space

    int cpu = -1; // measure on any cpu
    int group_fd = -1;
    unsigned long flags = 0;

    this->perf_fd = perf_event_open(&pe, pid, cpu, group_fd, flags);
    if (this->perf_fd == -1) {
        perror("perf_event_open");
        throw;
    }

    this->mp = (perf_event_mmap_page *)mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, this->perf_fd, 0);

    if (this->mp == MAP_FAILED) {
        perror("mmap");
        throw;
    }

    this->start();
}

int PT::decode_instruction(std::byte *buffer, size_t buffer_size) {
    struct pt_config config;
    struct pt_packet_decoder *decoder;
    struct pt_packet packet;
    int status;

    // Initialize the pt_config structure
    pt_config_init(&config);
    config.begin = buffer;
    config.end = buffer + buffer_size;

    // Create a new Intel PT packet decoder
    decoder = pt_pkt_alloc_decoder(&config);
    if (!decoder) {
        fprintf(stderr, "Error: Failed to allocate pt_packet_decoder.\n");
        return;
    }

    // Initialize the decoder
    status = pt_pkt_sync_forward(decoder);
    if (status < 0) {
        fprintf(stderr, "Error: Failed to synchronize pt_packet_decoder.\n");
        goto cleanup;
    }

    // Decode the Intel PT packets
    while (1) {
        status = pt_pkt_next(decoder, &packet, sizeof(packet));
        if (status < 0) {
            if (status == -pte_eos)
                break; // End of stream, exit the loop
            fprintf(stderr, "Error: Failed to decode packet.\n");
            break;
        }

        // Analyze the decoded packet
        // ...
    }

cleanup:
    // Clean up and deallocate the decoder
    pt_pkt_free_decoder(decoder);
}
int PT::read(CXLController *, struct PTElem *) {
    if (this->fd < 0) {
        return 0;
    }

    if (mp == MAP_FAILED)
        return -1;

    int r = 0;
    int i;
    struct perf_event_header *header;
    struct perf_sample *data;
    uint64_t last_head;
    char *dp = ((char *)mp) + PAGE_SIZE;

    do {
        this->seq = mp->lock;
        barrier();
        last_head = mp->data_head;
        while ((uint64_t)this->rdlen < last_head) {
            header = (struct perf_event_header *)(dp + this->rdlen % DATA_SIZE);

            switch (header->type) {
            case PERF_RECORD_LOST:
                LOG(DEBUG) << fmt::format("received PERF_RECORD_LOST\n");
                break;
            case PERF_RECORD_SAMPLE:
                data = (struct perf_sample *)(dp + this->rdlen % DATA_SIZE);

                if (header->size < sizeof(*data)) {
                    LOG(DEBUG) << fmt::format("size too small. size:{}\n", header->size);
                    r = -1;
                    continue;
                }
                if (this->pid == data->pid) {
//                    LOG(ERROR) << fmt::format("pid:{} tid:{} time:{} addr:{} phys_addr:{} llc_miss:{} timestamp={}\n",
//                                              data->pid, data->tid, data->time_enabled, data->addr, data->phys_addr,
//                                              data->value, data->timestamp);
//                    controller->insert(data->timestamp, data->phys_addr, data->addr, 0);
//                    elem->total++;
//                    elem->llcmiss = data->value;
                }
                break;
            case PERF_RECORD_THROTTLE:
                LOG(DEBUG) << "received PERF_RECORD_THROTTLE\n";
                break;
            case PERF_RECORD_UNTHROTTLE:
                LOG(DEBUG) << "received PERF_RECORD_UNTHROTTLE\n";
                break;
            case PERF_RECORD_LOST_SAMPLES:
                LOG(DEBUG) << "received PERF_RECORD_LOST_SAMPLES\n";
                break;
            default:
                LOG(DEBUG) << fmt::format("other data received. type:{}\n", header->type);
                break;
            }

            this->rdlen += header->size;
        }

        mp->data_tail = last_head;
        barrier();
    } while (mp->lock != this->seq);

    return r;
}
int PT::start() {
    if (this->perf_fd < 0) {
        return 0;
    }
    if (ioctl(this->perf_fd, PERF_EVENT_IOC_ENABLE, 0) < 0) {
        perror("ioctl");
        return -1;
    }
    return 0;
}
int PT::stop() {
    if (this->perf_fd < 0) {
        return 0;
    }
    if (ioctl(this->perf_fd, PERF_EVENT_IOC_DISABLE, 0) < 0) {
        perror("ioctl");
        return -1;
    }
    return 0;
}

PT::~PT() {
    this->stop();

    if (this->fd < 0) {
        return;
    }

    if (this->mp != MAP_FAILED) {
        munmap(this->mp, this->mplen);
        this->mp = reinterpret_cast<struct perf_event_mmap_page *>(MAP_FAILED);
        this->mplen = 0;
    }

    if (this->fd != -1) {
        close(this->fd);
        this->fd = -1;
    }

    this->pid = -1;
}