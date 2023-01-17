#!/bin/bash

SCRIPT=`(cd \`dirname $0\`; pwd)`
CURDIR=`pwd`
EDK2_DIR=${CURDIR}/build

EDK2_REPO=$HOME/workspace/uefi/tianocore
#EDK2_REPO=$HOME/workspace/uefi/riscv
EDK2_SRC=${EDK2_REPO}/edk2
EDK2_PLATFORMS_SRC=${EDK2_REPO}/edk2-platforms
#EDK2_PLAT=U540
EDK2_PLAT=Duowen

#export PACKAGES_PATH=${EDK2_SRC}:${EDK2_PLATFORMS_SRC}

if [ "x${EDK2_PLAT}" = "xU540" ]; then
	EDK2_DSC_PATH=SiFive/U5SeriesPkg/FreedomU540HiFiveUnleashedBoard
	EDK2_FD_PATH=FreedomU540HiFiveUnleashed
	EDK2_RISCV64_MISA=rv64imafdc
fi
if [ "x${EDK2_PLAT}" = "xDuowen" ]; then
	EDK2_DSC_PATH=SmarCo/DuowenPkg/DuowenSocBoard
	EDK2_FD_PATH=Duowen
	EDK2_RISCV64_MISA=rv64imafd
fi

echo "Creating repository ${EDK2_DIR}..."

rm -rf ${EDK2_DIR}
mkdir -p ${EDK2_DIR}
cp -rf ${EDK2_SRC}/* ${EDK2_DIR}/
cp -rf ${EDK2_PLATFORMS_SRC}/* ${EDK2_DIR}/
sed "s/\(^DEFINE\sGCC5_RISCV64_ARCH\s*=\s*\)\([0-9a-z_]*\)\(.*\)/\1${EDK2_RISCV64_MISA}\3/g" ${EDK2_DIR}/BaseTools/Conf/tools_def.template > ${EDK2_DIR}/BaseTools/Conf/tools_def.txt
mv ${EDK2_DIR}/BaseTools/Conf/tools_def.txt ${EDK2_DIR}/BaseTools/Conf/tools_def.template

(
cd ${EDK2_DIR}

export GCC5_RISCV64_PREFIX=riscv64-linux-

. ./edksetup.sh
make -C BaseTools clean
make -C BaseTools
make -C BaseTools/Source/C

. ./edksetup.sh BaseTools

export PATH=$PATH:${EDK2_DIR}/BaseTools/BinWrappers/PosixLike
export EDK_TOOLS_PATH=${EDK2_DIR}/BaseTools

EDK2_PLAT_UPPER=`echo ${EDK2_PLAT} | tr '[a-z]' '[A-Z]'`
build -a RISCV64 -p Platform/$EDK2_DSC_PATH/${EDK2_PLAT}.dsc -t GCC5
cp -v Build/${EDK2_FD_PATH}/DEBUG_GCC5/FV/${EDK2_PLAT_UPPER}.fd ../${EDK2_PLAT}.fd
)
