# Add path of toolchain to system path first, or enable ARM_GCC_TOOLCHAIN 
# -------------------------------------------------------------------
#ARM_GCC_TOOLCHAIN = toolchain/gcc-arm-none-eabi-4_9-2015q1/bin/

CROSS_COMPILE = $(ARM_GCC_TOOLCHAIN)arm-none-eabi-

# Compilation tools
AR = $(CROSS_COMPILE)ar
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)as
NM = $(CROSS_COMPILE)nm
LD = $(CROSS_COMPILE)gcc
GDB = $(CROSS_COMPILE)gdb
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

Q := @
# -------------------------------------------------------------------
# Initialize target name and target object files
# -------------------------------------------------------------------

LIB := librwnx.a

TARGET=Debug

OBJ_DIR=$(TARGET)
BIN_DIR=$(TARGET)

# -------------------------------------------------------------------
# Include folder list
# -------------------------------------------------------------------
INCLUDES =

INCLUDES += -I./config
INCLUDES += -I./beken378/common
INCLUDES += -I./beken378/demo
INCLUDES += -I./beken378/app
INCLUDES += -I./beken378/app/config
INCLUDES += -I./beken378/app/standalone-station
INCLUDES += -I./beken378/app/standalone-ap
INCLUDES += -I./beken378/ip/common
INCLUDES += -I./beken378/ip/ke/
INCLUDES += -I./beken378/ip/mac/
INCLUDES += -I./beken378/ip/lmac/src/hal
INCLUDES += -I./beken378/ip/lmac/src/mm
INCLUDES += -I./beken378/ip/lmac/src/ps
INCLUDES += -I./beken378/ip/lmac/src/rd
INCLUDES += -I./beken378/ip/lmac/src/rwnx
INCLUDES += -I./beken378/ip/lmac/src/rx
INCLUDES += -I./beken378/ip/lmac/src/scan
INCLUDES += -I./beken378/ip/lmac/src/sta
INCLUDES += -I./beken378/ip/lmac/src/tx
INCLUDES += -I./beken378/ip/lmac/src/vif
INCLUDES += -I./beken378/ip/lmac/src/rx/rxl
INCLUDES += -I./beken378/ip/lmac/src/tx/txl
INCLUDES += -I./beken378/ip/lmac/src/p2p
INCLUDES += -I./beken378/ip/lmac/src/chan
INCLUDES += -I./beken378/ip/lmac/src/td
INCLUDES += -I./beken378/ip/lmac/src/tpc
INCLUDES += -I./beken378/ip/lmac/src/tdls
INCLUDES += -I./beken378/ip/umac/src/mesh
INCLUDES += -I./beken378/ip/umac/src/rc
INCLUDES += -I./beken378/ip/umac/src/apm
INCLUDES += -I./beken378/ip/umac/src/bam
INCLUDES += -I./beken378/ip/umac/src/llc
INCLUDES += -I./beken378/ip/umac/src/me
INCLUDES += -I./beken378/ip/umac/src/rxu
INCLUDES += -I./beken378/ip/umac/src/scanu
INCLUDES += -I./beken378/ip/umac/src/sm
INCLUDES += -I./beken378/ip/umac/src/txu
INCLUDES += -I./beken378/driver/include
INCLUDES += -I./beken378/driver/common/reg
INCLUDES += -I./beken378/driver/entry
INCLUDES += -I./beken378/driver/dma
INCLUDES += -I./beken378/driver/intc
INCLUDES += -I./beken378/driver/phy
INCLUDES += -I./beken378/driver/rc_beken
INCLUDES += -I./beken378/driver/flash
INCLUDES += -I./beken378/driver/rw_pub
INCLUDES += -I./beken378/driver/common/reg
INCLUDES += -I./beken378/driver/common
INCLUDES += -I./beken378/driver/uart
INCLUDES += -I./beken378/driver/sys_ctrl
INCLUDES += -I./beken378/driver/gpio
INCLUDES += -I./beken378/driver/general_dma
INCLUDES += -I./beken378/driver/spidma
INCLUDES += -I./beken378/driver/icu
INCLUDES += -I./beken378/func/include
INCLUDES += -I./beken378/func/rf_test
INCLUDES += -I./beken378/func/user_driver
INCLUDES += -I./beken378/func/power_save
INCLUDES += -I./beken378/func/uart_debug
INCLUDES += -I./beken378/func/ethernet_intf
INCLUDES += -I./beken378/func/hostapd-2.5/hostapd
INCLUDES += -I./beken378/func/hostapd-2.5/bk_patch
INCLUDES += -I./beken378/func/hostapd-2.5/src/utils
INCLUDES += -I./beken378/func/hostapd-2.5/src/ap
INCLUDES += -I./beken378/func/hostapd-2.5/src/common
INCLUDES += -I./beken378/func/hostapd-2.5/src/drivers
INCLUDES += -I./beken378/func/hostapd-2.5/src
INCLUDES += -I./beken378/func/lwip_intf/lwip-2.0.2/src
INCLUDES += -I./beken378/func/lwip_intf/lwip-2.0.2/port
INCLUDES += -I./beken378/func/lwip_intf/lwip-2.0.2/src/include
INCLUDES += -I./beken378/func/lwip_intf/lwip-2.0.2/src/include/netif
INCLUDES += -I./beken378/func/lwip_intf/lwip-2.0.2/src/include/lwip
INCLUDES += -I./beken378/func/temp_detect
INCLUDES += -I./beken378/func/spidma_intf
INCLUDES += -I./beken378/func/rwnx_intf
INCLUDES += -I./beken378/func/joint_up
INCLUDES += -I./beken378/os/include
INCLUDES += -I./beken378/os/FreeRTOSv9.0.0/FreeRTOS/Source/portable/Keil/ARM968es
INCLUDES += -I./beken378/os/FreeRTOSv9.0.0/FreeRTOS/Source/include
INCLUDES += -I./beken378/os/FreeRTOSv9.0.0


INCLUDES += -I./beken378/driver/ble
INCLUDES += -I./beken378/driver/ble/ble_pub/ip/ble/hl/inc
INCLUDES += -I./beken378/driver/ble/ble_pub/ip/ble/profiles/comm/api
INCLUDES += -I./beken378/driver/ble/ble_pub/ip/ble/profiles/sdp/api
INCLUDES += -I./beken378/driver/ble/ble_pub/modules/app/api
INCLUDES += -I./beken378/driver/ble/ble_pub/modules/ecc_p256/api
INCLUDES += -I./beken378/driver/ble/ble_pub/modules/common/api
INCLUDES += -I./beken378/driver/ble/ble_pub/modules/dbg/api
INCLUDES += -I./beken378/driver/ble/ble_pub/modules/rwip/api
INCLUDES += -I./beken378/driver/ble/ble_pub/modules/rf/api
INCLUDES += -I./beken378/driver/ble/ble_pub/plf/refip/src/arch
INCLUDES += -I./beken378/driver/ble/ble_pub/plf/refip/src/arch/boot
INCLUDES += -I./beken378/driver/ble/ble_pub/plf/refip/src/arch/compiler
INCLUDES += -I./beken378/driver/ble/ble_pub/plf/refip/src/arch/ll
INCLUDES += -I./beken378/driver/ble/ble_pub/plf/refip/src/build/ble_full/reg/fw
INCLUDES += -I./beken378/driver/ble/ble_pub/plf/refip/src/driver/reg
INCLUDES += -I./beken378/driver/ble/ble_pub/plf/refip/src/driver/uart
INCLUDES += -I./beken378/driver/ble/ble_pub/plf/refip/src/driver/ble_icu

INCLUDES += -I./beken378/driver/ble/ble_lib/ip/ahi/api
INCLUDES += -I./beken378/driver/ble/ble_lib/ip/ble/hl/api
INCLUDES += -I./beken378/driver/ble/ble_lib/ip/ble/hl/inc
INCLUDES += -I./beken378/driver/ble/ble_lib/ip/ble/hl/src/gap/gapc
INCLUDES += -I./beken378/driver/ble/ble_lib/ip/ble/hl/src/gap/gapm
INCLUDES += -I./beken378/driver/ble/ble_lib/ip/ble/hl/src/gap/smpc
INCLUDES += -I./beken378/driver/ble/ble_lib/ip/ble/hl/src/gap/smpm
INCLUDES += -I./beken378/driver/ble/ble_lib/ip/ble/hl/src/gap
INCLUDES += -I./beken378/driver/ble/ble_lib/ip/ble/hl/src/gatt/attc
INCLUDES += -I./beken378/driver/ble/ble_lib/ip/ble/hl/src/gatt/attm
INCLUDES += -I./beken378/driver/ble/ble_lib/ip/ble/hl/src/gatt/atts
INCLUDES += -I./beken378/driver/ble/ble_lib/ip/ble/hl/src/gatt
INCLUDES += -I./beken378/driver/ble/ble_lib/ip/ble/hl/src/gatt/gattc
INCLUDES += -I./beken378/driver/ble/ble_lib/ip/ble/hl/src/gatt/gattm
INCLUDES += -I./beken378/driver/ble/ble_lib/ip/ble/hl/src/l2c/l2cc
INCLUDES += -I./beken378/driver/ble/ble_lib/ip/ble/hl/src/l2c/l2cm
INCLUDES += -I./beken378/driver/ble/ble_lib/ip/ble/ll/src/em
INCLUDES += -I./beken378/driver/ble/ble_lib/ip/ble/ll/src/llc
INCLUDES += -I./beken378/driver/ble/ble_lib/ip/ble/ll/src/lld
INCLUDES += -I./beken378/driver/ble/ble_lib/ip/ble/ll/src/llm
INCLUDES += -I./beken378/driver/ble/ble_lib/ip/ble/ll/src/rwble
INCLUDES += -I./beken378/driver/ble/ble_lib/ip/ea/api
INCLUDES += -I./beken378/driver/ble/ble_lib/ip/em/api
INCLUDES += -I./beken378/driver/ble/ble_lib/ip/hci/api
INCLUDES += -I./beken378/driver/ble/ble_lib/ip/hci/src
INCLUDES += -I./beken378/driver/ble/ble_lib/modules/h4tl/api
INCLUDES += -I./beken378/driver/ble/ble_lib/modules/ke/api
INCLUDES += -I./beken378/driver/ble/ble_lib/modules/ke/src
INCLUDES += -I./beken378/driver/ble/ble_pub/modules/ecc_p256/api

# -------------------------------------------------------------------
# Source file list
# -------------------------------------------------------------------
SRC_C =

#rwnx ip module
SRC_C += ./beken378/ip/common/co_math.c
SRC_C += ./beken378/ip/common/co_pool.c
SRC_C += ./beken378/ip/common/co_ring.c
SRC_C += ./beken378/ip/common/co_dlist.c
SRC_C += ./beken378/ip/common/co_list.c
SRC_C += ./beken378/ip/ke/ke_env.c
SRC_C += ./beken378/ip/ke/ke_event.c
SRC_C += ./beken378/ip/ke/ke_msg.c
SRC_C += ./beken378/ip/ke/ke_queue.c
SRC_C += ./beken378/ip/ke/ke_task.c
SRC_C += ./beken378/ip/ke/ke_timer.c
SRC_C += ./beken378/ip/lmac/src/chan/chan.c
SRC_C += ./beken378/ip/lmac/src/hal/hal_desc.c
SRC_C += ./beken378/ip/lmac/src/hal/hal_dma.c
SRC_C += ./beken378/ip/lmac/src/hal/hal_machw.c
SRC_C += ./beken378/ip/lmac/src/hal/hal_mib.c
SRC_C += ./beken378/ip/lmac/src/mm/mm.c
SRC_C += ./beken378/ip/lmac/src/mm/mm_bcn.c
SRC_C += ./beken378/ip/lmac/src/mm/mm_task.c
SRC_C += ./beken378/ip/lmac/src/mm/mm_timer.c
SRC_C += ./beken378/ip/lmac/src/p2p/p2p.c
SRC_C += ./beken378/ip/lmac/src/ps/ps.c
SRC_C += ./beken378/ip/lmac/src/rd/rd.c
SRC_C += ./beken378/ip/lmac/src/rwnx/rwnx.c
SRC_C += ./beken378/ip/lmac/src/rx/rx_swdesc.c
SRC_C += ./beken378/ip/lmac/src/rx/rxl/rxl_cntrl.c
SRC_C += ./beken378/ip/lmac/src/rx/rxl/rxl_hwdesc.c
SRC_C += ./beken378/ip/lmac/src/scan/scan.c
SRC_C += ./beken378/ip/lmac/src/scan/scan_shared.c
SRC_C += ./beken378/ip/lmac/src/scan/scan_task.c
SRC_C += ./beken378/ip/lmac/src/sta/sta_mgmt.c
SRC_C += ./beken378/ip/lmac/src/td/td.c
SRC_C += ./beken378/ip/lmac/src/tdls/tdls.c
SRC_C += ./beken378/ip/lmac/src/tdls/tdls_task.c
SRC_C += ./beken378/ip/lmac/src/tpc/tpc.c
SRC_C += ./beken378/ip/lmac/src/tx/tx_swdesc.c
SRC_C += ./beken378/ip/lmac/src/tx/txl/txl_buffer.c
SRC_C += ./beken378/ip/lmac/src/tx/txl/txl_buffer_shared.c
SRC_C += ./beken378/ip/lmac/src/tx/txl/txl_cfm.c
SRC_C += ./beken378/ip/lmac/src/tx/txl/txl_cntrl.c
SRC_C += ./beken378/ip/lmac/src/tx/txl/txl_frame.c
SRC_C += ./beken378/ip/lmac/src/tx/txl/txl_frame_shared.c
SRC_C += ./beken378/ip/lmac/src/tx/txl/txl_hwdesc.c
SRC_C += ./beken378/ip/lmac/src/vif/vif_mgmt.c
SRC_C += ./beken378/ip/mac/mac.c
SRC_C += ./beken378/ip/mac/mac_ie.c
SRC_C += ./beken378/ip/umac/src/apm/apm.c
SRC_C += ./beken378/ip/umac/src/apm/apm_task.c
SRC_C += ./beken378/ip/umac/src/bam/bam.c
SRC_C += ./beken378/ip/umac/src/bam/bam_task.c
SRC_C += ./beken378/ip/umac/src/me/me.c
SRC_C += ./beken378/ip/umac/src/me/me_mgmtframe.c
SRC_C += ./beken378/ip/umac/src/me/me_mic.c
SRC_C += ./beken378/ip/umac/src/me/me_task.c
SRC_C += ./beken378/ip/umac/src/me/me_utils.c
SRC_C += ./beken378/ip/umac/src/rc/rc.c
SRC_C += ./beken378/ip/umac/src/rc/rc_basic.c
SRC_C += ./beken378/ip/umac/src/rxu/rxu_cntrl.c
SRC_C += ./beken378/ip/umac/src/scanu/scanu.c
SRC_C += ./beken378/ip/umac/src/scanu/scanu_shared.c
SRC_C += ./beken378/ip/umac/src/scanu/scanu_task.c
SRC_C += ./beken378/ip/umac/src/sm/sm.c
SRC_C += ./beken378/ip/umac/src/sm/sm_task.c
SRC_C += ./beken378/ip/umac/src/txu/txu_cntrl.c 

# Generate obj list
# -------------------------------------------------------------------
OBJ_LIST = $(SRC_C:%.c=$(OBJ_DIR)/%.o)
DEPENDENCY_LIST = $(SRC_C:%.c=$(OBJ_DIR)/%.d)

# Compile options
# -------------------------------------------------------------------
CFLAGS =
CFLAGS += -g -mthumb -mcpu=arm968e-s -march=armv5te -mthumb-interwork -mlittle-endian -Os -std=c99 -ffunction-sections -Wall -fsigned-char -fdata-sections -Wunknown-pragmas

$(LIB): $(OBJ_LIST)
	$(AR) -rcs $(BIN_DIR)/$@ $(OBJ_LIST)

# Generate build info
# -------------------------------------------------------------------	

$(OBJ_DIR)/%.o: %.c
	$(Q)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(Q)echo CC $<
	$(Q)$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	$(Q)$(CC) $(CFLAGS) $(INCLUDES) -c $< -MM -MT $@ -MF $(patsubst %.o,%.d,$@)

-include $(DEPENDENCY_LIST)
	
.PHONY: clean
clean:
	rm -rf $(TARGET)
