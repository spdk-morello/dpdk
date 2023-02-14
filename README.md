# DPDK for Arm Morello

The 'morello' branch of this repository contains changes to DPDK release v22.11 to support native builds on the Arm Morello platform running CheriBSD.

## Prerequisites

The examples below assume the use of 'sudo' which can be installed and configured as 'root' using:

~~~{.sh}
pkg64c install sudo
visudo
~~~

To perform a native build on CheriBSD, the following packages are required:

~~~{.sh}
sudo pkg64 install meson llvm llvm-base py39-pip libelf gdb-cheri
sudo pkg64c install git libelf
pip install pyelftools
~~~

## Source Code

The modified DPDK source code can be obtained using:
~~~{.sh}
git clone https://github.com/spdk-morello/dpdk
cd dpdk
git checkout morello
~~~

## CheriBSD Source Code

To build the DPDK kernel module, the source for CheriBSD needs to be installed in /usr/src:

~~~{.sh}
cd /usr/src
sudo git clone https://github.com/CTSRD-CHERI/cheribsd .
~~~

If .gitmodules does not exist, create it with the following contents:

~~~{.sh}
[submodule "sys/contrib/subrepo-openzfs/scripts/zfs-images"]
  path = sys/contrib/subrepo-openzfs/scripts/zfs-images
  url = https://github.com/CTSRD-CHERI/zfs
  branch = cheri-hybrid
~~~

Initialise the submodules:

~~~{.sh}
sudo git submodule update --init
~~~

If the ZFS submodule fails to initialise, then extract the code manually:

~~~{.sh}
cd sys/contrib/subrepo-openzfs
sudo git checkout cheri-hybrid
~~~

## Kernel Build

DPDK kernel modules are required to manage the allocation of physically contiguous memory and for direct access to PCI devices.

These can be built with:

~~~{.sh}
CC=clang meson -Dexamples=helloworld -Denable_kmods=true build-kern
MACHINE_ARCH=aarch64 ZFSTOP=/usr/src/sys/contrib/subrepo-openzfs ninja -j4 -C build-kern
~~~

The modules can be installed in /boot/modules using:

~~~{.sh}
sudo cp build-kern/kernel/freebsd/*.ko /boot/modules
sudo kldxref /boot/modules
~~~

The kernel modules need to be loaded with:

~~~{.sh}
sudo kldload contigmem nic_uio
~~~

To load them automatically after every boot, add the following line to /etc/rc.conf:

~~~{.sh}
kld_load="contigmem nic_uio"
~~~

## Hybrid Build

To create a hybrid build:

~~~{.sh}
CC=clang meson -Dexamples=helloworld build-hybrid
ninja -j4 -C build-hybrid
~~~

For a hybrid debug build:

~~~{.sh}
CC=clang meson -Dexamples=helloworld --buildtype=debug build-hybrid-debug
ninja -j4 -C build-hybrid-debug
~~~

To run 'helloworld':

~~~{.sh}
sudo ./build-hybrid/examples/dpdk-helloworld
~~~

The hybrid build generates the expected results from 'helloworld'. No further testing has been done.

## PureCap Build

To create a purecap build:

~~~{.sh}
CC=cc meson -Dexamples=helloworld build-pure
ninja -j4 -C build-pure
~~~

For a purecap debug build:

~~~{.sh}
CC=cc meson -Dexamples=helloworld --buildtype=debug build-pure-debug
ninja -j4 -C build-pure-debug
~~~

The purecap build produces multiple warnings that need investigation, especially related to increased structure sizes due to the increased size of a pointer.

The 'helloworld' example fails during initialisation with an EPROT error due to a mmap handling issue:

~~~{.sh}
sudo ./build-pure/examples/dpdk-helloworld
~~~

## Known Issues

A number of hacks have been used in order to move past blocking problems. These changes, which will need to be revisited, have been identified with:

~~~{.sh}
RTE_ARCH_ARM_MORELLO_HACK
RTE_ARCH_ARM_PURECAP_HACK
~~~
