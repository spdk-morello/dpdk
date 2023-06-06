# DPDK for Arm Morello

The 'morello' branch of this repository contains changes to DPDK release v22.11 to support native builds on the Arm Morello platform running CheriBSD.

This repository is not intended to be a permanent fork. It provides a convenient place to make work in progress visible to the community but the intent is upstream as much as possible.

[rtegrity](https://rtegrity.com/) is leading the ongoing development of the project, as part of the [Digital Security by Design](https://www.dsbd.tech/) Technology Access Programme.

## Prerequisites

The examples below assume the use of 'sudo' which can be installed and configured as 'root' using:

~~~{.sh}
pkg64c install sudo
visudo
~~~

To perform a native build on CheriBSD, the following packages are required:

~~~{.sh}
sudo pkg64 install -y meson ninja python llvm llvm-base py39-pip gdb-cheri
sudo pkg64c install -y git libelf
pip install pyelftools
~~~

## Source Code

The patched DPDK source code can be obtained using:
~~~{.sh}
git clone https://github.com/spdk-morello/dpdk
cd dpdk
git checkout morello
~~~

## CheriBSD Source Code

To build the DPDK kernel modules, the source for CheriBSD needs to be installed in /usr/src:

~~~{.sh}
sudo git clone https://github.com/CTSRD-CHERI/cheribsd /usr/src
~~~

With CheriBSD 22.12, kernel modules fail to build with 'is ZFSTOP set?'.

As a temporary workaround, run the following after installing the source:

~~~{.sh}
echo 'ZFSTOP=${SYSDIR}/contrib/subrepo-openzfs' | sudo tee -a /usr/share/mk/bsd.sysdir.mk
~~~

Memory allocated with the contigmem kernel module does not support storing capabilities.
To work around this, the temporary hack in kernel/freebsd/contigmem/cheribsd.patch can be
applied:

~~~{.sh}
sudo patch -d /usr/src < kernel/freebsd/contigmem/cheribsd.patch
~~~

The kernel can then be rebuilt and installed following the instructions in
https://github.com/CTSRD-CHERI/cheripedia/wiki/HOWTO:-Build-CheriBSD-natively-on-Morello.

## Kernel Modules

DPDK kernel modules are required to manage the allocation of physically contiguous memory and for direct access to PCI devices.

These can be built by including '-Denable_kmods=true' on the 'meson' command line as shown in the examples below. Note that the modules need to match the type of kernel that is running (hybrid or purecap), which may be different to the type of the required DPDK libraries.

The modules can be installed in /boot/modules using:

~~~{.sh}
sudo cp <build-path>/kernel/freebsd/*.ko /boot/modules
sudo kldxref /boot/modules
~~~

The kernel modules need to be loaded with:

~~~{.sh}
sudo kldload contigmem nic_uio
~~~

To load them automatically after every boot, add the following line to /etc/rc.conf:

~~~{.sh}
kld_list="contigmem nic_uio"
~~~

## Hybrid Build

To create a hybrid build:

~~~{.sh}
export PKG_CONFIG_PATH=`pwd`/config/hybrid
CC=clang meson -Dexamples=helloworld -Denable_kmods=true build-hybrid
ninja -j4 -C build-hybrid
~~~

For a hybrid debug build:

~~~{.sh}
export PKG_CONFIG_PATH=`pwd`/config/hybrid
CC=clang meson -Dexamples=helloworld -Denable_kmods=true --buildtype=debug build-hybrid-debug
ninja -j4 -C build-hybrid-debug
~~~

## Purecap Build

To create a purecap build:

~~~{.sh}
export PKG_CONFIG_PATH=`pwd`/config/purecap
CC=cc meson -Dexamples=helloworld -Denable_kmods=true build-pure
ninja -j4 -C build-pure
~~~

To reduce the number of warnings, specify -Wno-cheri-inefficient:

~~~{.sh}
export PKG_CONFIG_PATH=`pwd`/config/purecap
CC=cc CFLAGS=-Wno-cheri-inefficient meson -Dexamples=helloworld -Denable_kmods=true build-pure
ninja -j4 -C build-pure
~~~

For a purecap debug build:

~~~{.sh}
export PKG_CONFIG_PATH=`pwd`/config/purecap
CC=cc CFLAGS=-Wno-cheri-inefficient meson -Dexamples=helloworld -Denable_kmods=true --buildtype=debug build-pure-debug
ninja -j4 -C build-pure-debug
~~~

The purecap build produces multiple warnings that need investigation, especially related to increased structure sizes due to the increased size of a pointer.

## Testing

To run 'helloworld':

~~~{.sh}
sudo <build-path>/examples/dpdk-helloworld
~~~

The hybrid build generates the expected results from 'helloworld'. No further testing has been done.

The purecap build fails during initialisation with an EPROT error due to a mmap handling issue:

## Known Issues

A number of hacks have been used in order to move past blocking problems. These changes, which will need to be revisited, have been identified with:

~~~{.sh}
RTE_ARCH_ARM_MORELLO_HACK
RTE_ARCH_ARM_PURECAP_HACK
~~~

## Acknowledgements

[rtegrity](https://rtegrity.com/) has been able to develop this project thanks to the support of the [Digital Security by Design](https://www.dsbd.tech/) Technology Access Programme.

## Core Maintainers

The [core maintainers](https://github.com/spdk-morello/dpdk/blob/main/MAINTAINERS.md) primary responsibility is to provide technical oversight for the DPDK-Morello project.
The current list includes:
* [Nick Connolly](https://github.com/nconnolly1), [rtegrity](https://rtegrity.com/)s