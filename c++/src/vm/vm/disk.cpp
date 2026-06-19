#ifndef VDISK_CPP
#define VDISK_CPP

#include "disk.hpp"


VirtualDisk::VirtualDisk(std::vector<uint8_t> &dma_ref, int communication_addr) : dma(dma_ref) 
{
    this->communication_addr = communication_addr;
    this->mode_code_addr = communication_addr + this->mode_offset;
    this->mem_addr_addr = communication_addr + this->mem_addr_offset;
    this->disk_addr_addr = communication_addr + this->disk_addr_offset;
    this->size_addr = communication_addr + this->size_offset;
    this->return_addr = communication_addr + this->return_offset;
    this->ready_flag_addr = communication_addr + this->ready_flag_offset;
    this->done_flag_addr = communication_addr + this->done_flag_offset;
    this->busy_flag_addr = communication_addr + this->busy_flag_offset;
}

void VirtualDisk::read()
{
    uint64_t mem_addr = this->dma[this->mem_addr_addr];
    uint64_t disk_addr = this->dma[this->disk_addr_addr];
    uint64_t size = this->dma[this->size_addr];

    std::cout << "mem_addr: " << mem_addr << std::endl;
    std::cout << "disk_addr: " << disk_addr << std::endl;
    std::cout << "size: " << size << std::endl;

    for (int i = 0; i < size; i++)
    {
        this->dma[mem_addr+i] = this->disk_mem[disk_addr+i];
    }

    this->dma[this->return_addr] = size;
    return;
}

void VirtualDisk::write()
{
    uint64_t mem_addr = this->dma[this->mem_addr_addr];
    uint64_t disk_addr = this->dma[this->disk_addr_addr];
    uint64_t size = this->dma[this->size_addr];

    for (int i = 0; i < size; i++)
    {
        this->disk_mem[disk_addr+i] = this->dma[mem_addr+i];
    }

    this->dma[this->return_addr] = size;
    return;
}

void VirtualDisk::step()
{
    std::cout << "vdisk step" << std::endl;
    int ready_flag = static_cast<int>(this->dma[this->ready_flag_addr]);
    int busy_flag = static_cast<int>(this->dma[this->busy_flag_addr]);
    std::cout << "ready_flag: " << ready_flag << std::endl;
    std::cout << "busy_flag: " << busy_flag << std::endl;
    if ((ready_flag != 1) || (busy_flag == 1))
    {
        return;
    }
    
    this->dma[this->busy_flag_addr] = 1;
    this->dma[this->done_flag_addr] = 0;
    this->dma[this->ready_flag_addr] = 0;

    int mode_code = static_cast<int>(this->dma[this->mode_code_addr]);

    switch (mode_code)
    {
    case (int)DiskModeCodes::READ:
        std::cout << "vdisk read" << std::endl;
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


void VirtualDisk::loadImg()
{
    std::string path = "./vdisk/disk.img";
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file)
    {
        throw std::runtime_error("Failed to open binary file: " + path);
    }

    std::streamsize file_size = file.tellg();
    if (file_size < 0 || file_size % sizeof(uint8_t) != 0)
    {
        throw std::runtime_error("Invalid binary file size: " + path);
    }

    std::vector<uint8_t> bytes(static_cast<size_t>(file_size) / sizeof(uint8_t));
    file.seekg(0, std::ios::beg);
    file.read(
        reinterpret_cast<char*>(bytes.data()),
        bytes.size() * sizeof(uint8_t)
    );

    if (!file)
    {
        throw std::runtime_error("Failed to read binary file: " + path);
    }

    for (int i = 0; (i < bytes.size()) && (i < this->DISK_SIZE); i++)
    {
        this->disk_mem[i] = bytes[i];
    }
    return;
}

void VirtualDisk::safeImg()
{
    std::string path = "./vdisk/disk.img";
    std::ofstream file(path, std::ios::binary | std::ios::out);

    if (!file)
    {
        throw std::runtime_error("Failed to open binary file: " + path);
    }

    file.write(
        reinterpret_cast<const char*>(this->disk_mem.data()),
        static_cast<std::streamsize>(this->disk_mem.size())
    );
}


#endif