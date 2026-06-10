#ifndef VDISK_CPP
#define VDISK_CPP

#include "disk.hpp"


VirtualDisk::VirtualDisk(std::vector<uint64_t>& dma_ref, int communication_addr) : dma(dma_ref) 
{
    this->communication_addr = communication_addr;
    this->mode_addr = communication_addr + this->mode_offset;
    this->mem_begin_addr = communication_addr + this->mem_range_begin_offset;
    this->mem_end_addr = communication_addr + this->mem_range_end_offset;
    this->disk_begin_addr = communication_addr + this->disk_begin_offset;
    this->disk_end_addr = communication_addr + this->disk_end_offset;
    this->return_addr = communication_addr + this->return_offset;
    this->ready_flag_addr = communication_addr + this->ready_flag_offset;
    this->done_flag_addr = communication_addr + this->done_flag_offset;
    this->busy_flag_addr = communication_addr + this->busy_flag_offset;
}

void VirtualDisk::read()
{
    uint64_t mem_range_begin = this->dma[this->mem_begin_addr];
    uint64_t mem_range_end = this->dma[this->mem_end_addr];

    uint64_t disk_range_begin = this->dma[this->disk_begin_addr];
    uint64_t disk_range_end = this->dma[this->disk_end_addr];

    uint64_t i_mem = mem_range_begin;
    uint64_t i_disk = disk_range_begin;

    while ((i_mem < mem_range_end) && i_disk < (disk_range_end))
    {
        uint64_t word = 0;

        for (int b = 0; b < 8 && i_disk < disk_range_end; b++)
        {
            word = (word << 8) | (uint64_t)this->disk_mem[i_disk++];
        }

        this->dma[i_mem++] = word;
    }

    this->dma[this->return_addr] = i_disk;
}

void VirtualDisk::write()
{
    uint64_t mem_range_begin = this->dma[this->mem_begin_addr];
    uint64_t mem_range_end = this->dma[this->mem_end_addr];

    uint64_t disk_range_begin = this->dma[this->disk_begin_addr];
    uint64_t disk_range_end = this->dma[this->disk_end_addr];

    uint64_t i_mem = mem_range_begin;
    uint64_t i_disk = disk_range_begin;

    while ((i_mem < mem_range_end) && (i_disk < disk_range_end))
    {
        uint64_t word = this->dma[i_mem++];

        for (int b = 7; (b >= 0) && (i_disk < disk_range_end); b--)
        {
            uint8_t byte = (word >> (b * 8)) & 0xFF;
            this->disk_mem[i_disk++] = byte;
        }
    }

    this->dma[this->return_addr] = i_disk;
}

void VirtualDisk::step()
{
    int ready_flag = static_cast<int>(this->dma[this->ready_flag_addr]);
    int busy_flag = static_cast<int>(this->dma[this->busy_flag_addr]);
    if ((ready_flag != 1) || (busy_flag == 1))
    {
        return;
    }
    
    this->dma[this->busy_flag_addr] = 1;
    this->dma[this->done_flag_addr] = 0;
    this->dma[this->ready_flag_addr] = 0;

    int mode_code = static_cast<int>(this->dma[this->mode_addr]);

    switch (mode_code)
    {
    case (int)DiskModeCodes::READ:
        this->read();
        break;
    case (int)DiskModeCodes::WRITE:
        this->write();
        break;
    default:
        this->dma[this->busy_flag_addr] = 0;
        this->dma[this->return_addr] = -1;
        break;
    }

    this->dma[this->busy_flag_addr] = 0;
    this->dma[this->done_flag_addr] = 1;
    return;
}

#endif