import os
import subprocess

class binBuildInfo:
    def __init__(self,out_path: str, name: str, disk_location: int = -1):
        self.source_files: list[str] = []
        self.out_path: str = out_path
        self.name: str = name
        self.path: str = self.out_path + "/" + self.name
        self.disk_location: int = disk_location

    def setSourceFiles(self, files):
        # print(files)
        for file in files:
            if (os.path.exists(file) == False) or (file.endswith(".asm") == False):
                print(f"Invalid source file path: {file}")
                exit(1)
            self.source_files.append(file)
        

    def compile(self):
        files = " ".join(self.source_files)
        cmd = f"../../c++/out/VM.exe -link {files} -o {self.path}"
        print(cmd)
        subprocess.run(cmd)


class diskBuilder:
    def __init__(self, bins: list[binBuildInfo]):
        self.DISK_SIZE = 64 * 1024 * 1024 # 64 MB
        self.disk_mem = bytearray(self.DISK_SIZE)
        self.bins = bins

    def build(self):
        for binary in self.bins:
            binary.compile()

            with open(binary.path, "rb") as f:
                bytes = f.read()

            if (binary.disk_location == -1):
                raise ValueError(f"Assign a disk location to {binary.path}")

            end = binary.disk_location + len(bytes)
            if (self.DISK_SIZE < end):
                raise ValueError(f"{binary.path} does not fit on disk")

            self.disk_mem[binary.disk_location : end] = bytes
        




bootloader_build = binBuildInfo("./bins", "bootloader.bin")
bootloader_build.setSourceFiles(["../src/bootloader.asm",
                                "../src/fileIO/io.asm"])
bootloader_build.compile()

# db = diskBuilder([bootloader_build])
# with open(bootloader_build.path, "rb") as f:
#     print(f.read())