#ifndef VDISK_HPP
#define VDISK_HPP

#include <cstring>
#include "../types.hpp"

enum class DiskModeCodes
{
    READ = 1,
    WRITE = 2,
};

class VirtualDisk
{
    private:
    const int DISK_SIZE = 64000000;
    std::vector<uint8_t> disk_mem = std::vector<uint8_t>(DISK_SIZE); // 64 MB
    std::vector<bool> dirty = std::vector<bool>(DISK_SIZE);

    std::vector<uint64_t>& dma;
    
    uint64_t communication_addr;
    uint64_t mode_offset = 0;
    uint64_t mem_range_begin_offset = 1;
    uint64_t mem_range_end_offset = 2;
    uint64_t disk_begin_offset = 3;
    uint64_t disk_end_offset = 4;
    uint64_t return_offset = 5;
    uint64_t ready_flag_offset = 6;
    uint64_t busy_flag_offset = 7;
    uint64_t done_flag_offset = 8;
    
    uint64_t mode_addr;
    uint64_t mem_begin_addr;
    uint64_t mem_end_addr;
    uint64_t disk_begin_addr;
    uint64_t disk_end_addr;
    uint64_t return_addr;
    uint64_t ready_flag_addr;
    uint64_t busy_flag_addr;
    uint64_t done_flag_addr;

    void read();
    void write();

    public:
    VirtualDisk(std::vector<uint64_t>& dma_ref, int communication_addr);
    void step();
};

#endif