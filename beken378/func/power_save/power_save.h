#ifndef _POWER_SAVE_H_
#define _POWER_SAVE_H_

#include "param_config.h"
#include "co_list.h"
#include "power_save_pub.h"
#include "ps_debug_pub.h"

typedef enum
{
    STA_GET_INIT = 0,
    STA_GET_TRUE = 1,
    STA_GET_FALSE = 2,
    STA_GET_TIMEOUT = 3,
} PS_STA_BEACON_STATE;

typedef enum
{
    PS_ARM_WAKEUP_NONE = 0,
    PS_ARM_WAKEUP_RW = 1,
} PS_ARM_WAKEUP_WAY;

typedef enum
{
    PS_LISTEN_MODE_DTIM = 0,
    PS_LISTEN_MODE_INTERVAL = 1,
} PS_LISTEN_MODE;

typedef struct ps_do_wkup_sem
{
    beken_semaphore_t wkup_sema;
    struct co_list_hdr list;
} PS_DO_WKUP_SEM;

typedef struct  ps_sta
{
    PS_ARM_WAKEUP_WAY ps_arm_wakeup_way ;
    UINT8     ps_real_sleep ;
    UINT8 sleep_first;
    UINT8 ps_can_sleep;
    UINT8 if_wait_bcn;
    UINT8 liston_int;
    PS_LISTEN_MODE liston_mode;
    UINT8 pwm0_clkmux;
    UINT8 ps_dtim_period;
    UINT8 ps_dtim_count;
    UINT8 ps_dtim_multi;
    volatile PS_STA_BEACON_STATE waited_beacon;
    UINT8 ps_bcn_loss_cnt;
    UINT16 ps_beacon_int;
    UINT16 PsPeriWakeupWaitTimeMs ;
    UINT16 sleep_ms;
    UINT32 nxmac_timer_v;
    UINT32 pwm0_less_time;
    UINT32 sleep_count ;
    UINT32 next_ps_time;
    struct co_list wk_list;
} STA_PS_INFO;


__INLINE struct ps_do_wkup_sem *list2sem(struct co_list_hdr const *l_list)
{
    return (struct ps_do_wkup_sem *) (((uint8_t *)l_list) - offsetof(struct ps_do_wkup_sem, list));
}

void power_save_mac_idle_callback(void);
UINT32 power_save_wkup_event_get(void);
void power_save_dtim_ps_init();
void power_save_ieee_dtim_wakeup(void);
UINT8 power_save_me_ps_set_all_state(UINT8 state);
PS_STA_BEACON_STATE power_save_beacon_state_get(void);
PS_ARM_WAKEUP_WAY power_save_wkup_way_get(void);
void power_save_set_uart_linger_time(UINT32 uart_wakeup_time);
extern void bmsg_ps_sender(uint8_t ioctl);
extern void ps_fake_data_rx_check(void);
extern bool ps_sleep_check(void);
extern u8 rwn_mgmt_is_only_sta_role_add(void);
extern void power_save_beacon_state_set(PS_STA_BEACON_STATE state);
extern void power_save_wait_timer_init(void);
extern void power_save_keep_timer_stop(void);
extern void power_save_wait_timer_stop(void);

//#define  PS_NEXT_DATA_CK_TM    2500 //5s

#define PS_STA_DTIM_SWITCH (power_save_if_ps_rf_dtim_enabled() \
                            &&g_wlan_general_param->role == CONFIG_ROLE_STA)


#define PS_STA_DTIM_CAN_SLEEP (PS_STA_DTIM_SWITCH          \
                && power_save_beacon_state_get() == STA_GET_TRUE    \
                && power_save_if_ps_can_sleep())
                
#endif // _POWER_SAVE_H_
// eof

