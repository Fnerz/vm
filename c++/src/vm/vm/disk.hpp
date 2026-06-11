#ifndef VDISK_HPP
#define VDISK_HPP

#include <cstring>
#include <string>
#include <fstream>
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

    std::vector<uint8_t>& dma;
    
    uint64_t communication_addr;
    uint64_t mode_offset = 0;
    uint64_t mem_addr_offset = 1;
    uint64_t disk_addr_offset = 2;
    uint64_t size_offset = 3;
    uint64_t return_offset = 4;
    uint64_t ready_flag_offset = 5;
    uint64_t busy_flag_offset = 6;
    uint64_t done_flag_offset = 7;
    
    uint64_t mode_code_addr;
    uint64_t mem_addr_addr;
    uint64_t disk_addr_addr;
    uint64_t size_addr;
    uint64_t return_addr;
    uint64_t ready_flag_addr;
    uint64_t busy_flag_addr;
    uint64_t done_flag_addr;

    void read();
    void write();

    public:
    VirtualDisk(std::vector<uint8_t> &dma_ref, int communication_addr);
    void loadImg();
    void safeImg();
    void step();
};

#endif