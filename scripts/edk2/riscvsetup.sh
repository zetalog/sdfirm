#!/bin/sh

#EDK2_PLATFORM=riscv-edk2-platforms
EDK2_PLATFORMS=edk2-platforms

mkfs.msdos -C linux.iso 64000
sudo losetup /dev/loop0 linux.iso
sudo mount /dev/loop0 /mnt

# Copy the kernel and initramfs that you built previously
sudo cp linux-riscv64.efi /mnt
sudo cp initramfs.cpio /mnt
sudo umount /mnt
sudo losetup -d /dev/loop0

# Copy ramdisk and DTB into your clone of edk2-platforms
#cp linux.iso ${EDK2_PLATFORMS}/Silicon/RISC-V/ProcessorPkg/Universal/EspRamdisk/linux.iso
#cp U540.dtb ${EDK2_PLATFORMS}/Platform/RISC-V/PlatformPkg/Universal/Sec/Riscv64/U540.dtb
