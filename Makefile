#
# make KDIR=xxxx SDP=1406 CROSS_COMPILE=yyy
#

KDIR?=/opt/vd/kernel/hawk/linux-3.10.28.Open
CROSS_COMPILE?=/opt/vd/toolchains/armv7l-tizen-20140701/bin/armv7l-tizen-linux-gnueabi-

# 1404, 1406, 1501
SDP?=1406

ccflags-y += -DCONFIG_ARCH_SDP${SDP}=1

ifeq (y, $(filter y, ${CONFIG_ARCH_SDP1404} ${CONFIG_ARCH_SDP1406}))
	ccflags-y += -I${PWD}/samsung/hawk
	ccflags-y += -I${PWD}/samsung/hawk/sdp_tsd/tsd/sdp${SDP}
endif

ifeq (${CONFIG_ARCH_SDP1501}, y)
	ccflags-y += -I${PWD}/samsung/jazz
	ccflags-y += -I${PWD}/samsung/jazz/sdp_tsd/tsd/sdp1501
endif

ifeq (${CONFIG_OCM}, y)
	ccflags-y += -DSDP_TSD_OCM_EVAL
endif

ccflags-y += -I${KDIR}
ccflags-y += -I${KDIR}/drivers/media/dvb-core
ccflags-y += -I${KDIR}/drivers/media/platform/sdp-dvb
ccflags-y += -I${KDIR}/include/uapi/linux/dvb

ccflags-y += -Wno-unused-variable
ccflags-y += -Wno-unused-function
ccflags-y += -Wno-declaration-after-statement


obj-m += sgodvb-sdp${SDP}.o
sgodvb-sdp${SDP}-y += sgo_dvb.o
sgodvb-sdp${SDP}-y += hijack.o utils.o llist.o sdp_util.o

all:
	make -C $(KDIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE="${CROSS_COMPILE}" SDP=${SDP}

clean:
	make -C $(KDIR) M=$(PWD) clean ARCH=arm CROSS_COMPILE="${CROSS_COMPILE}" SDP=${SDP}
