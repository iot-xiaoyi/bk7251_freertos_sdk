#Usage in source directory(CMake 3.13 above):
#cmake -S. -Bbuild -G"Unix Makefiles"
#Usage in build directory:
#cmake -S.. -G"Unix Makefiles"

cmake_minimum_required(VERSION 3.13)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER "arm-none-eabi-gcc") 
set(CMAKE_CXX_COMPILER "arm-none-eabi-g++")
set(CMAKE_C_AR "arm-none-eabi-ar")  
set(CMAKE_ASM_COMPILER "arm-none-eabi-gcc") 

set(CMAKE_OBJCOPY "arm-none-eabi-objcopy")
set(CMAKE_SIZE "arm-none-eabi-size")

# Look for includes and libraries only in the target system prefix.
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Disable compiler checks.
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

project(bk7231u)

enable_language(C ASM)

set(exe_target bk7231u)
set(beken_prj_dir "${CMAKE_SOURCE_DIR}/demos")
set(beken_sdk_dir "${CMAKE_SOURCE_DIR}/beken378")


# Compile definitions/macros
set(beken_defines
    -D__little_endian__=1
)

# Compiler flags
set(beken_c_flags
    "-mcpu=arm968e-s"
    "-march=armv5te"
    "-marm"
    "-mthumb-interwork"
    "-mlittle-endian"
    "-g"
    "-Os"
    "-fdata-sections"
    "-ffunction-sections"
    #"-ffreestanding"
    #"-fno-strict-aliasing"
    #"-fno-common"
    "-gdwarf-2"
    "-nostdlib"
    "-std=c99"
    "-fsigned-char"
    #"-Wall"
    #"-Wextra"
    #"-Wno-unused-parameter"
    #"-Wno-empty-body"
    #"-Wno-missing-field-initializers"
    #"-Wno-sign-compare"
    #"-Wno-type-limits"
)

set(beken_asm_flags
    "-mcpu=arm968e-s"
    "-march=armv5te"
    "-marm"
    "-g"
    "-Os"
    "-x assembler-with-cpp"
)

set(beken_cxx_flags
    "-mcpu=arm968e-s"
    "-march=armv5te"
    "-marm"
    "-mthumb-interwork"
    "-mlittle-endian"
    "-g"
    "-Os"
    "-fdata-sections"
    "-ffunction-sections"
    "-fomit-frame-pointer"
    "-mabi=aapcs"
    "-fno-unroll-loops"
    "-ffast-math"
    "-ftree-vectorize"
    "-std=c++11"
)

# Linker flags
set(beken_ld_flags
    "--specs=nosys.specs"
    "-g"
    "-marm"
    "-mcpu=arm968e-s"
    "-nostartfiles"
    "-Wl,--gc-sections"
    #"-Wl,--wrap,malloc"
    #"-Wl,--wrap,free"
    #"-Wl,--wrap,calloc"
    #"-Wl,--wrap,realloc"
    #"-Wl,--wrap,_malloc_r"
    #"-Wl,--wrap,_free_r"
    #"-Wl,--wrap,_calloc_r"
    #"-Wl,--wrap,_realloc_r"
    #"-Wl,--wrap,printf"
    #"-Wl,--wrap,fprintf"
)

# Xlinker flags
set (CMAKE_C_LINKER_WRAPPER_FLAG "-Xlinker" " ")
#add_link_options("LINKER:--undefined")
#add_link_options("LINKER:uxTopUsedPriority")
#add_link_options("LINKER:--cref")
add_link_options("LINKER:--gc-sections")
#add_link_options("LINKER:--defsym=_rom_data=64")
add_link_options("LINKER:-M")
add_link_options("LINKER:-Map")
add_link_options("LINKER:${exe_target}.map")

set(beken_wifi_inc
    "${beken_sdk_dir}/ip/common"
    "${beken_sdk_dir}/ip/ke/"
    "${beken_sdk_dir}/ip/mac/"
    "${beken_sdk_dir}/ip/lmac/src/hal"
    "${beken_sdk_dir}/ip/lmac/src/mm"
    "${beken_sdk_dir}/ip/lmac/src/ps"
    "${beken_sdk_dir}/ip/lmac/src/rd"
    "${beken_sdk_dir}/ip/lmac/src/rwnx"
    "${beken_sdk_dir}/ip/lmac/src/rx"
    "${beken_sdk_dir}/ip/lmac/src/scan"
    "${beken_sdk_dir}/ip/lmac/src/sta"
    "${beken_sdk_dir}/ip/lmac/src/tx"
    "${beken_sdk_dir}/ip/lmac/src/vif"
    "${beken_sdk_dir}/ip/lmac/src/rx/rxl"
    "${beken_sdk_dir}/ip/lmac/src/tx/txl"
    "${beken_sdk_dir}/ip/lmac/src/p2p"
    "${beken_sdk_dir}/ip/lmac/src/chan"
    "${beken_sdk_dir}/ip/lmac/src/td"
    "${beken_sdk_dir}/ip/lmac/src/tpc"
    "${beken_sdk_dir}/ip/lmac/src/tdls"
    "${beken_sdk_dir}/ip/umac/src/mesh"
    "${beken_sdk_dir}/ip/umac/src/rc"
    "${beken_sdk_dir}/ip/umac/src/apm"
    "${beken_sdk_dir}/ip/umac/src/bam"
    "${beken_sdk_dir}/ip/umac/src/llc"
    "${beken_sdk_dir}/ip/umac/src/me"
    "${beken_sdk_dir}/ip/umac/src/rxu"
    "${beken_sdk_dir}/ip/umac/src/scanu"
    "${beken_sdk_dir}/ip/umac/src/sm"
    "${beken_sdk_dir}/ip/umac/src/txu"
)

set(beken_wifi_src
    "${beken_sdk_dir}/ip/common/co_dlist.c"
    "${beken_sdk_dir}/ip/common/co_list.c"
    "${beken_sdk_dir}/ip/common/co_math.c"
    "${beken_sdk_dir}/ip/common/co_pool.c"
    "${beken_sdk_dir}/ip/common/co_ring.c"
    "${beken_sdk_dir}/ip/ke/ke_env.c"
    "${beken_sdk_dir}/ip/ke/ke_event.c"
    "${beken_sdk_dir}/ip/ke/ke_msg.c"
    "${beken_sdk_dir}/ip/ke/ke_queue.c"
    "${beken_sdk_dir}/ip/ke/ke_task.c"
    "${beken_sdk_dir}/ip/ke/ke_timer.c"
    "${beken_sdk_dir}/ip/lmac/src/chan/chan.c"
    "${beken_sdk_dir}/ip/lmac/src/hal/hal_desc.c"
    "${beken_sdk_dir}/ip/lmac/src/hal/hal_dma.c"
    "${beken_sdk_dir}/ip/lmac/src/hal/hal_machw.c"
    "${beken_sdk_dir}/ip/lmac/src/hal/hal_mib.c"
    "${beken_sdk_dir}/ip/lmac/src/mm/mm.c"
    "${beken_sdk_dir}/ip/lmac/src/mm/mm_bcn.c"
    "${beken_sdk_dir}/ip/lmac/src/mm/mm_task.c"
    "${beken_sdk_dir}/ip/lmac/src/mm/mm_timer.c"
    "${beken_sdk_dir}/ip/lmac/src/p2p/p2p.c"
    "${beken_sdk_dir}/ip/lmac/src/ps/ps.c"
    "${beken_sdk_dir}/ip/lmac/src/rd/rd.c"
    "${beken_sdk_dir}/ip/lmac/src/rwnx/rwnx.c"
    "${beken_sdk_dir}/ip/lmac/src/rx/rx_swdesc.c"
    "${beken_sdk_dir}/ip/lmac/src/rx/rxl/rxl_cntrl.c"
    "${beken_sdk_dir}/ip/lmac/src/rx/rxl/rxl_hwdesc.c"
    "${beken_sdk_dir}/ip/lmac/src/scan/scan.c"
    "${beken_sdk_dir}/ip/lmac/src/scan/scan_shared.c"
    "${beken_sdk_dir}/ip/lmac/src/scan/scan_task.c"
    "${beken_sdk_dir}/ip/lmac/src/sta/sta_mgmt.c"
    "${beken_sdk_dir}/ip/lmac/src/td/td.c"
    "${beken_sdk_dir}/ip/lmac/src/tdls/tdls.c"
    "${beken_sdk_dir}/ip/lmac/src/tdls/tdls_task.c"
    "${beken_sdk_dir}/ip/lmac/src/tpc/tpc.c"
    "${beken_sdk_dir}/ip/lmac/src/tx/tx_swdesc.c"
    "${beken_sdk_dir}/ip/lmac/src/tx/txl/txl_buffer.c"
    "${beken_sdk_dir}/ip/lmac/src/tx/txl/txl_buffer_shared.c"
    "${beken_sdk_dir}/ip/lmac/src/tx/txl/txl_cfm.c"
    "${beken_sdk_dir}/ip/lmac/src/tx/txl/txl_cntrl.c"
    "${beken_sdk_dir}/ip/lmac/src/tx/txl/txl_frame.c"
    "${beken_sdk_dir}/ip/lmac/src/tx/txl/txl_frame_shared.c"
    "${beken_sdk_dir}/ip/lmac/src/tx/txl/txl_hwdesc.c"
    "${beken_sdk_dir}/ip/lmac/src/vif/vif_mgmt.c"
    "${beken_sdk_dir}/ip/mac/mac.c"
    "${beken_sdk_dir}/ip/mac/mac_ie.c"
    "${beken_sdk_dir}/ip/umac/src/apm/apm.c"
    "${beken_sdk_dir}/ip/umac/src/apm/apm_task.c"
    "${beken_sdk_dir}/ip/umac/src/bam/bam.c"
    "${beken_sdk_dir}/ip/umac/src/bam/bam_task.c"
    "${beken_sdk_dir}/ip/umac/src/me/me.c"
    "${beken_sdk_dir}/ip/umac/src/me/me_mgmtframe.c"
    "${beken_sdk_dir}/ip/umac/src/me/me_mic.c"
    "${beken_sdk_dir}/ip/umac/src/me/me_task.c"
    "${beken_sdk_dir}/ip/umac/src/me/me_utils.c"
    "${beken_sdk_dir}/ip/umac/src/rc/rc.c"
    "${beken_sdk_dir}/ip/umac/src/rc/rc_basic.c"
    "${beken_sdk_dir}/ip/umac/src/rxu/rxu_cntrl.c"
    "${beken_sdk_dir}/ip/umac/src/scanu/scanu.c"
    "${beken_sdk_dir}/ip/umac/src/scanu/scanu_shared.c"
    "${beken_sdk_dir}/ip/umac/src/scanu/scanu_task.c"
    "${beken_sdk_dir}/ip/umac/src/sm/sm.c"
    "${beken_sdk_dir}/ip/umac/src/sm/sm_task.c"
    "${beken_sdk_dir}/ip/umac/src/txu/txu_cntrl.c"
)

set(beken_ble_pub_inc
    "${beken_sdk_dir}/driver/ble"
    "${beken_sdk_dir}/driver/ble/ble_pub/ip/ble/hl/inc"
    "${beken_sdk_dir}/driver/ble/ble_pub/ip/ble/profiles/comm/api"
    "${beken_sdk_dir}/driver/ble/ble_pub/ip/ble/profiles/sdp/api"
    "${beken_sdk_dir}/driver/ble/ble_pub/modules/app/api"
    "${beken_sdk_dir}/driver/ble/ble_pub/modules/ecc_p256/api"
    "${beken_sdk_dir}/driver/ble/ble_pub/modules/common/api"
    "${beken_sdk_dir}/driver/ble/ble_pub/modules/dbg/api"
    "${beken_sdk_dir}/driver/ble/ble_pub/modules/rwip/api"
    "${beken_sdk_dir}/driver/ble/ble_pub/modules/rf/api"
    "${beken_sdk_dir}/driver/ble/ble_pub/plf/refip/src/arch"
    "${beken_sdk_dir}/driver/ble/ble_pub/plf/refip/src/arch/boot"
    "${beken_sdk_dir}/driver/ble/ble_pub/plf/refip/src/arch/compiler"
    "${beken_sdk_dir}/driver/ble/ble_pub/plf/refip/src/arch/ll"
    "${beken_sdk_dir}/driver/ble/ble_pub/plf/refip/src/build/ble_full/reg/fw"
    "${beken_sdk_dir}/driver/ble/ble_pub/plf/refip/src/driver/reg"
    "${beken_sdk_dir}/driver/ble/ble_pub/plf/refip/src/driver/uart"
    "${beken_sdk_dir}/driver/ble/ble_pub/plf/refip/src/driver/ble_icu"
)

set(beken_ble_pub_src
    "${beken_sdk_dir}/driver/ble/ble.c"
    "${beken_sdk_dir}/driver/ble/ble_pub/ip/ble/hl/src/prf/prf.c"
    "${beken_sdk_dir}/driver/ble/ble_pub/ip/ble/profiles/comm/src/comm.c"
    "${beken_sdk_dir}/driver/ble/ble_pub/ip/ble/profiles/comm/src/comm_task.c"
    "${beken_sdk_dir}/driver/ble/ble_pub/ip/ble/profiles/sdp/src/sdp_service.c"
    "${beken_sdk_dir}/driver/ble/ble_pub/ip/ble/profiles/sdp/src/sdp_service_task.c"
    "${beken_sdk_dir}/driver/ble/ble_pub/modules/app/src/app_ble.c"
    "${beken_sdk_dir}/driver/ble/ble_pub/modules/app/src/app_sec.c"
    "${beken_sdk_dir}/driver/ble/ble_pub/modules/app/src/app_task.c"
    "${beken_sdk_dir}/driver/ble/ble_pub/modules/app/src/app_sdp.c"
    "${beken_sdk_dir}/driver/ble/ble_pub/modules/app/src/app_comm.c"
    "${beken_sdk_dir}/driver/ble/ble_pub/modules/ecc_p256/src/ecc_p256.c"
    "${beken_sdk_dir}/driver/ble/ble_pub/modules/common/src/common_list.c"
    "${beken_sdk_dir}/driver/ble/ble_pub/modules/common/src/common_utils.c"
    "${beken_sdk_dir}/driver/ble/ble_pub/modules/common/src/RomCallFlash.c"
    "${beken_sdk_dir}/driver/ble/ble_pub/modules/dbg/src/dbg.c"
    "${beken_sdk_dir}/driver/ble/ble_pub/modules/dbg/src/dbg_mwsgen.c"
    "${beken_sdk_dir}/driver/ble/ble_pub/modules/dbg/src/dbg_swdiag.c"
    "${beken_sdk_dir}/driver/ble/ble_pub/modules/dbg/src/dbg_task.c"
    "${beken_sdk_dir}/driver/ble/ble_pub/modules/rwip/src/rwip.c"
    "${beken_sdk_dir}/driver/ble/ble_pub/modules/rf/src/ble_rf_xvr.c"
    "${beken_sdk_dir}/driver/ble/ble_pub/plf/refip/src/driver/uart/uart.c"
)

set(beken_ble_lib_inc
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ahi/api"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/api"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/inc"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gap/gapc"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gap/gapm"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gap/smpc"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gap/smpm"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gap"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gatt/attc"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gatt/attm"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gatt/atts"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gatt"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gatt/gattc"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gatt/gattm"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/l2c/l2cc"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/l2c/l2cm"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/ll/src/em"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/ll/src/llc"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/ll/src/lld"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/ll/src/llm"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/ll/src/rwble"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ea/api"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/em/api"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/hci/api"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/hci/src"
    "${beken_sdk_dir}/driver/ble/ble_lib/modules/h4tl/api"
    "${beken_sdk_dir}/driver/ble/ble_lib/modules/ke/api"
    "${beken_sdk_dir}/driver/ble/ble_lib/modules/ke/src"
    "${beken_sdk_dir}/driver/ble/ble_pub/modules/ecc_p256/api"
)

set(beken_ble_lib_src
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ahi/src/ahi.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ahi/src/ahi_task.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gap/gapc/gapc.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gap/gapc/gapc_hci.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gap/gapc/gapc_sig.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gap/gapc/gapc_task.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gap/gapm/gapm.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gap/gapm/gapm_hci.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gap/gapm/gapm_task.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gap/gapm/gapm_util.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gap/smpc/smpc.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gap/smpc/smpc_api.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gap/smpc/smpc_crypto.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gap/smpc/smpc_util.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gap/smpm/smpm_api.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gatt/attc/attc.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gatt/attm/attm.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gatt/attm/attm_db.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gatt/atts/atts.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gatt/gattc/gattc.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gatt/gattc/gattc_task.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gatt/gattm/gattm.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/gatt/gattm/gattm_task.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/l2c/l2cc/l2cc.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/l2c/l2cc/l2cc_lecb.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/l2c/l2cc/l2cc_pdu.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/l2c/l2cc/l2cc_sig.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/l2c/l2cc/l2cc_task.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/l2c/l2cm/l2cm.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/prf/prf_utils.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/hl/src/rwble_hl/rwble_hl.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/ll/src/em/em_buf.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/ll/src/llc/llc.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/ll/src/llc/llc_ch_asses.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/ll/src/llc/llc_hci.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/ll/src/llc/llc_llcp.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/ll/src/llc/llc_task.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/ll/src/llc/llc_util.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/ll/src/lld/lld.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/ll/src/lld/lld_evt.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/ll/src/lld/lld_pdu.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/ll/src/lld/lld_sleep.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/ll/src/lld/lld_util.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/ll/src/lld/lld_wlcoex.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/ll/src/llm/llm.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/ll/src/llm/llm_hci.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/ll/src/llm/llm_task.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/ll/src/llm/llm_util.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ble/ll/src/rwble/rwble.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/ea/src/ea.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/hci/src/hci.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/hci/src/hci_fc.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/hci/src/hci_msg.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/hci/src/hci_tl.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/ip/hci/src/hci_util.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/modules/h4tl/src/h4tl.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/modules/ke/src/kernel.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/modules/ke/src/kernel_event.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/modules/ke/src/kernel_mem.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/modules/ke/src/kernel_msg.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/modules/ke/src/kernel_queue.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/modules/ke/src/kernel_task.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/modules/ke/src/kernel_timer.c"
    "${beken_sdk_dir}/driver/ble/ble_lib/plf/refip/src/arch/main/ble_arch_main.c"
)

set(beken_sdk_inc
    "${beken_sdk_dir}/app"
    "${beken_sdk_dir}/app/config"
    "${beken_sdk_dir}/app/http"
    "${beken_sdk_dir}/app/standalone-station"
    "${beken_sdk_dir}/app/standalone-ap"
    "${beken_sdk_dir}/common"
    "${beken_sdk_dir}/demo"
    "${beken_sdk_dir}/driver/include"
    "${beken_sdk_dir}/driver/common/reg"
    "${beken_sdk_dir}/driver/entry"
    "${beken_sdk_dir}/driver/dma"
    "${beken_sdk_dir}/driver/intc"
    "${beken_sdk_dir}/driver/phy"
    "${beken_sdk_dir}/driver/rc_beken"
    "${beken_sdk_dir}/driver/flash"
    "${beken_sdk_dir}/driver/rw_pub"
    "${beken_sdk_dir}/driver/common/reg"
    "${beken_sdk_dir}/driver/common"
    "${beken_sdk_dir}/driver/uart"
    "${beken_sdk_dir}/driver/sys_ctrl"
    "${beken_sdk_dir}/driver/gpio"
    "${beken_sdk_dir}/driver/general_dma"
    "${beken_sdk_dir}/driver/spidma"
    "${beken_sdk_dir}/driver/icu"
    "${beken_sdk_dir}/func/include"
    "${beken_sdk_dir}/func/rf_test"
    "${beken_sdk_dir}/func/user_driver"
    "${beken_sdk_dir}/func/power_save"
    "${beken_sdk_dir}/func/uart_debug"
    "${beken_sdk_dir}/func/easy_flash"
    "${beken_sdk_dir}/func/easy_flash/inc"
    "${beken_sdk_dir}/func/easy_flash/port"
    "${beken_sdk_dir}/func/ethernet_intf"
    "${beken_sdk_dir}/func/hostapd-2.5/hostapd"
    "${beken_sdk_dir}/func/hostapd-2.5/bk_patch"
    "${beken_sdk_dir}/func/hostapd-2.5/src/utils"
    "${beken_sdk_dir}/func/hostapd-2.5/src/ap"
    "${beken_sdk_dir}/func/hostapd-2.5/src/common"
    "${beken_sdk_dir}/func/hostapd-2.5/src/drivers"
    "${beken_sdk_dir}/func/hostapd-2.5/src"
    "${beken_sdk_dir}/func/temp_detect"
    "${beken_sdk_dir}/func/spidma_intf"
    "${beken_sdk_dir}/func/rwnx_intf"
    "${beken_sdk_dir}/func/joint_up"
    "${beken_sdk_dir}/os/include"
    "${beken_sdk_dir}/os/FreeRTOSv9.0.0"
    "${beken_sdk_dir}/os/FreeRTOSv9.0.0/FreeRTOS/Source/include"
    "${beken_sdk_dir}/os/FreeRTOSv9.0.0/FreeRTOS/Source/portable/Keil/ARM968es"
    "${beken_prj_dir}/demos"
    "${beken_prj_dir}/demos/net/uart1_tcp_server"
    "${beken_prj_dir}"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/include"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/include/lwip"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/port"
)

set(beken_sdk_src
    "${beken_sdk_dir}/app/app.c"
    "${beken_sdk_dir}/app/ate_app.c"
    "${beken_sdk_dir}/app/config/param_config.c"
    "${beken_sdk_dir}/app/standalone-ap/sa_ap.c"
    "${beken_sdk_dir}/app/standalone-station/sa_station.c"
    "${beken_sdk_dir}/app/http/utils_httpc.c"
    "${beken_sdk_dir}/app/http/utils_net.c"
    "${beken_sdk_dir}/app/http/utils_timer.c"
    "${beken_sdk_dir}/app/http/lite-log.c"
    "${beken_sdk_dir}/app/tftp/tftpclient.c"
    "${beken_sdk_dir}/demo/ieee802_11_demo.c"
    "${beken_sdk_dir}/driver/common/dd.c"
    "${beken_sdk_dir}/driver/common/drv_model.c"
    "${beken_sdk_dir}/driver/dma/dma.c"
    "${beken_sdk_dir}/driver/driver.c"
    "${beken_sdk_dir}/driver/entry/arch_main.c"
    "${beken_sdk_dir}/driver/fft/fft.c"
    "${beken_sdk_dir}/driver/flash/flash.c"
    "${beken_sdk_dir}/driver/general_dma/general_dma.c"
    "${beken_sdk_dir}/driver/gpio/gpio.c"
    "${beken_sdk_dir}/driver/i2s/i2s.c"
    "${beken_sdk_dir}/driver/icu/icu.c"
    "${beken_sdk_dir}/driver/intc/intc.c"
    "${beken_sdk_dir}/driver/irda/irda.c"
    "${beken_sdk_dir}/driver/macphy_bypass/mac_phy_bypass.c"
    "${beken_sdk_dir}/driver/phy/phy_trident.c"
    "${beken_sdk_dir}/driver/pwm/pwm.c"
    "${beken_sdk_dir}/driver/pwm/mcu_ps_timer.c"
    "${beken_sdk_dir}/driver/pwm/bk_timer.c"
    "${beken_sdk_dir}/driver/rw_pub/rw_platf_pub.c"
    "${beken_sdk_dir}/driver/saradc/saradc.c"
    "${beken_sdk_dir}/driver/spi/spi.c"
    "${beken_sdk_dir}/driver/spidma/spidma.c"
    "${beken_sdk_dir}/driver/sys_ctrl/sys_ctrl.c"
    "${beken_sdk_dir}/driver/uart/Retarget.c"
    "${beken_sdk_dir}/driver/uart/uart.c"
    "${beken_sdk_dir}/driver/wdt/wdt.c"
    "${beken_sdk_dir}/func/func.c"
    "${beken_sdk_dir}/func/airkiss/bk_airkiss.c"
    "${beken_sdk_dir}/func/airkiss/airkiss_main.c"
    "${beken_sdk_dir}/func/bk7011_cal/bk7231_cal.c"
    "${beken_sdk_dir}/func/bk7011_cal/manual_cal_bk7231.c"
    "${beken_sdk_dir}/func/bk7011_cal/bk7231U_cal.c"
    "${beken_sdk_dir}/func/bk7011_cal/manual_cal_bk7231U.c"
    "${beken_sdk_dir}/func/joint_up/role_launch.c"
    "${beken_sdk_dir}/func/joint_up/reconnect_startup.c"
    "${beken_sdk_dir}/func/hostapd_intf/hostapd_intf.c"
    "${beken_sdk_dir}/func/hostapd-2.5/bk_patch/ddrv.c"
    "${beken_sdk_dir}/func/hostapd-2.5/bk_patch/signal.c"
    "${beken_sdk_dir}/func/hostapd-2.5/bk_patch/sk_intf.c"
    "${beken_sdk_dir}/func/hostapd-2.5/bk_patch/fake_socket.c"
    "${beken_sdk_dir}/func/hostapd-2.5/hostapd/main_none.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/crypto/aes-internal.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/crypto/aes-internal-dec.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/crypto/aes-internal-enc.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/crypto/aes-unwrap.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/crypto/aes-wrap.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/crypto/md5.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/crypto/md5-internal.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/crypto/rc4.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/crypto/sha1.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/crypto/sha1-internal.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/crypto/sha1-pbkdf2.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/crypto/sha1-prf.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/crypto/tls_none.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/ap/ap_config.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/ap/ap_drv_ops.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/ap/ap_list.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/ap/ap_mlme.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/ap/authsrv.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/ap/beacon.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/ap/bss_load.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/ap/dfs.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/ap/drv_callbacks.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/ap/eap_user_db.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/ap/hostapd.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/ap/hw_features.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/ap/ieee802_11_auth.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/ap/ieee802_11.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/ap/ieee802_11_ht.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/ap/ieee802_11_shared.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/ap/ieee802_1x.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/ap/pmksa_cache_auth.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/ap/sta_info.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/ap/tkip_countermeasures.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/ap/utils.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/ap/wmm.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/ap/wpa_auth.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/ap/wpa_auth_glue.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/ap/wpa_auth_ie.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/common/hw_features_common.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/common/ieee802_11_common.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/common/wpa_common.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/drivers/driver_beken.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/drivers/driver_common.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/drivers/drivers.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/eap_common/eap_common.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/eapol_auth/eapol_auth_sm.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/eap_server/eap_server.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/eap_server/eap_server_methods.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/l2_packet/l2_packet_none.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/rsn_supp/preauth.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/rsn_supp/wpa.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/rsn_supp/wpa_ie.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/utils/common.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/utils/eloop.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/utils/os_none.c"
    "${beken_sdk_dir}/func/hostapd-2.5/src/utils/wpabuf.c"
    "${beken_sdk_dir}/func/hostapd-2.5/wpa_supplicant/ap.c"
    "${beken_sdk_dir}/func/hostapd-2.5/wpa_supplicant/blacklist.c"
    "${beken_sdk_dir}/func/hostapd-2.5/wpa_supplicant/bss.c"
    "${beken_sdk_dir}/func/hostapd-2.5/wpa_supplicant/config.c"
    "${beken_sdk_dir}/func/hostapd-2.5/wpa_supplicant/config_none.c"
    "${beken_sdk_dir}/func/hostapd-2.5/wpa_supplicant/eap_register.c"
    "${beken_sdk_dir}/func/hostapd-2.5/wpa_supplicant/events.c"
    "${beken_sdk_dir}/func/hostapd-2.5/wpa_supplicant/main_supplicant.c"
    "${beken_sdk_dir}/func/hostapd-2.5/wpa_supplicant/notify.c"
    "${beken_sdk_dir}/func/hostapd-2.5/wpa_supplicant/wmm_ac.c"
    "${beken_sdk_dir}/func/hostapd-2.5/wpa_supplicant/wpa_scan.c"
    "${beken_sdk_dir}/func/hostapd-2.5/wpa_supplicant/wpas_glue.c"
    "${beken_sdk_dir}/func/hostapd-2.5/wpa_supplicant/wpa_supplicant.c"
    "${beken_sdk_dir}/func/misc/fake_clock.c"
    "${beken_sdk_dir}/func/misc/target_util.c"
    "${beken_sdk_dir}/func/power_save/power_save.c"
    "${beken_sdk_dir}/func/power_save/manual_ps.c"
    "${beken_sdk_dir}/func/power_save/mcu_ps.c"
    "${beken_sdk_dir}/func/rf_test/rx_sensitivity.c"
    "${beken_sdk_dir}/func/rf_test/tx_evm.c"
    "${beken_sdk_dir}/func/rwnx_intf/rw_ieee80211.c"
    "${beken_sdk_dir}/func/rwnx_intf/rw_msdu.c"
    "${beken_sdk_dir}/func/rwnx_intf/rw_msg_rx.c"
    "${beken_sdk_dir}/func/rwnx_intf/rw_msg_tx.c"
    "${beken_sdk_dir}/func/sim_uart/gpio_uart.c"
    "${beken_sdk_dir}/func/sim_uart/pwm_uart.c"
    "${beken_sdk_dir}/func/spidma_intf/spidma_intf.c"
    "${beken_sdk_dir}/func/temp_detect/temp_detect.c"
    "${beken_sdk_dir}/func/uart_debug/cmd_evm.c"
    "${beken_sdk_dir}/func/uart_debug/cmd_help.c"
    "${beken_sdk_dir}/func/uart_debug/cmd_reg.c"
    "${beken_sdk_dir}/func/uart_debug/cmd_rx_sensitivity.c"
    "${beken_sdk_dir}/func/uart_debug/command_line.c"
    "${beken_sdk_dir}/func/uart_debug/command_table.c"
    "${beken_sdk_dir}/func/uart_debug/udebug.c"
    "${beken_sdk_dir}/func/user_driver/BkDriverFlash.c"
    "${beken_sdk_dir}/func/user_driver/BkDriverGpio.c"
    "${beken_sdk_dir}/func/user_driver/BkDriverPwm.c"
    "${beken_sdk_dir}/func/user_driver/BkDriverUart.c"
    "${beken_sdk_dir}/func/user_driver/BkDriverWdg.c"
    "${beken_sdk_dir}/func/wlan_ui/extral_cli.c"
    "${beken_sdk_dir}/func/wlan_ui/wlan_cli.c"
    "${beken_sdk_dir}/func/wlan_ui/wlan_ui.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/port/ethernetif.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/port/net.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/port/sys_arch.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/api/api_lib.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/api/api_msg.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/api/err.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/api/netbuf.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/api/netdb.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/api/netifapi.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/api/sockets.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/api/tcpip.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/def.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/dns.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/inet_chksum.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/init.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/ip.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/ipv4/dhcp.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/ipv4/etharp.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/ipv4/icmp.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/ipv4/igmp.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/ipv4/ip4_addr.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/ipv4/ip4.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/ipv4/ip4_frag.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/ipv6/dhcp6.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/ipv6/ethip6.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/ipv6/icmp6.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/ipv6/inet6.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/ipv6/ip6_addr.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/ipv6/ip6.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/ipv6/ip6_frag.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/ipv6/mld6.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/ipv6/nd6.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/mem.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/memp.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/netif.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/pbuf.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/raw.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/stats.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/sys.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/tcp.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/tcp_in.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/tcp_out.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/timeouts.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/core/udp.c"
    "${beken_sdk_dir}/func/lwip_intf/lwip-2.0.2/src/netif/ethernet.c"
    "${beken_sdk_dir}/func/lwip_intf/dhcpd/dhcp-server.c"
    "${beken_sdk_dir}/func/lwip_intf/dhcpd/dhcp-server-main.c"
    "${beken_sdk_dir}/func/lwip_intf/tcp_server.c"
    "${beken_sdk_dir}/func/lwip_intf/platform_stub.c"
    "${beken_sdk_dir}/os/FreeRTOSv9.0.0/FreeRTOS/Source/portable/Keil/ARM968es/port.c"
    "${beken_sdk_dir}/os/FreeRTOSv9.0.0/FreeRTOS/Source/portable/MemMang/heap_4.c"
    "${beken_sdk_dir}/os/FreeRTOSv9.0.0/FreeRTOS/Source/list.c"
    "${beken_sdk_dir}/os/FreeRTOSv9.0.0/FreeRTOS/Source/queue.c"
    "${beken_sdk_dir}/os/FreeRTOSv9.0.0/FreeRTOS/Source/tasks.c"
    "${beken_sdk_dir}/os/FreeRTOSv9.0.0/FreeRTOS/Source/timers.c"
    "${beken_sdk_dir}/os/FreeRTOSv9.0.0/rtos_pub.c"
    "${beken_sdk_dir}/os/mem_arch.c"
    "${beken_sdk_dir}/os/str_arch.c"
    "${beken_sdk_dir}/driver/entry/boot_handlers.S"
    "${beken_sdk_dir}/driver/entry/boot_vectors.S"
)

add_library(beken_wifi STATIC ${beken_wifi_src})
set_target_properties(beken_wifi PROPERTIES OUTPUT_NAME "ip_7231u")
target_include_directories(beken_wifi PRIVATE ${beken_wifi_inc} ${beken_sdk_inc})
target_compile_options(beken_wifi PRIVATE ${beken_defines} ${beken_c_flags})
# move /Y libip_7231u.a ..\beken378\ip\libip_7231u.a
if (OFF)
add_custom_command(
    TARGET beken_wifi POST_BUILD
    COMMAND "echo" "Running Post-build step"
    COMMAND "${CMAKE_COMMAND}" -E copy "libip_7231u.a" "${beken_sdk_dir}/ip/libip_7231u.a"
)
endif()

add_library(beken_ble STATIC ${beken_ble_lib_src})
set_target_properties(beken_ble PROPERTIES OUTPUT_NAME "ble")
target_include_directories(beken_ble PRIVATE ${beken_wifi_inc} ${beken_sdk_inc} ${beken_ble_pub_inc} ${beken_ble_lib_inc})
target_compile_options(beken_ble PRIVATE ${beken_defines} ${beken_c_flags})
# move /Y libble.a ..\beken378\driver\ble\libble.a
if (OFF)
add_custom_command(
    TARGET beken_ble POST_BUILD
    COMMAND "echo" "Running Post-build step"
    COMMAND "${CMAKE_COMMAND}" -E copy "libble.a" "${beken_sdk_dir}/driver/ble/libble.a"
)
endif()

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------
# We require you to define at least demos and tests executable targets. Available demos and tests
# will be automatically enabled by us. You need to provide other project settings such as linker
# scripts and post build commands.

# ==================== Example ====================
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

set(default_modules beken_wifi beken_ble)
add_executable(${exe_target} ${beken_sdk_src} ${beken_ble_pub_src} ${app_src} ${beken_wifi_src})
target_include_directories(${exe_target} PRIVATE ${beken_wifi_inc} ${beken_sdk_inc} ${beken_ble_pub_inc} ${beken_ble_lib_inc})
target_compile_options(${exe_target} PRIVATE ${beken_defines} ${beken_c_flags})
target_link_options(${exe_target} BEFORE PRIVATE ${beken_ld_flags})
target_link_libraries(${exe_target}
  PRIVATE
    -Wl,--start-group
#    "${beken_sdk_dir}/drvier/ble/libble.a"
#    "${beken_sdk_dir}/ip/libip_7231u.a"
    ${default_modules}
    -Wl,--end-group
    -T ${beken_sdk_dir}/build/bk7231.ld
)

# Convert afx file to bin file
#configure_file("${AFR_ROOT_DIR}/projects/beken/tools/beken_packager.exe" "beken_packager.exe" COPYONLY)
#configure_file("${AFR_ROOT_DIR}/projects/beken/tools/config.json" "config.json" COPYONLY)
#configure_file("${beken_prj_dir}/bootloader_bk7231u_aws_v1.0.bin" "bootloader_bk7231u_aws_v1.0.bin" COPYONLY)

set(output_bin_file "$<TARGET_FILE_DIR:${exe_target}>/${exe_target}.bin")
set(output_elf_file "$<TARGET_FILE_DIR:${exe_target}>/${exe_target}.elf")

if (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
set(encrypt_tool "${CMAKE_SOURCE_DIR}/tool/crc binary/encrypt.exe")
else()
set(encrypt_tool "${CMAKE_SOURCE_DIR}/tool/crc binary/encrypt")
endif()

add_custom_command(
    TARGET ${exe_target} POST_BUILD
    COMMAND "echo" "Running Post-build step"
    COMMAND "${CMAKE_COMMAND}" -E remove "bk7231u.bin"
    COMMAND "${CMAKE_OBJCOPY}" -O binary "${output_elf_file}" "bk7231u.bin"
    COMMAND "${CMAKE_SIZE}" "${output_elf_file}"
    COMMAND "${encrypt_tool}" "bk7231u.bin" "00000000"
)

set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "bk7231u.bin" "${exe_target}.map")
