#include "include.h"
#include "fake_clock_pub.h"
#include "pwm_pub.h"
#include "icu_pub.h"
#include "drv_model_pub.h"
#include "uart_pub.h"

#if CFG_SUPPORT_ALIOS
#include "rtos_pub.h"
#include "ll.h"
#include "k_api.h"
#elif (CFG_SUPPORT_RTT)
#include <rtthread.h>
#else
#include "sys_rtos.h"
#include "arm_arch.h"
#endif
#include "bk_timer_pub.h"
#include "power_save_pub.h"

#if CFG_BK7221_MDM_WATCHDOG_PATCH
void rc_reset_patch(void);
#endif

static volatile UINT32 current_clock = 0;
static volatile UINT32 current_seconds = 0;
static UINT32 second_countdown = FCLK_SECOND;

#define         ONE_CAL_TIME        1000
typedef struct
{
    UINT32 fclk_tick;
    UINT32 tmp1;
} CAL_TICK_T;
static CAL_TICK_T cal_tick_save;
UINT32 use_cal_net = 0;

static void fclk_hdl(UINT8 param)
{
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
#if 0//CFG_USE_STA_PS
    if(power_save_use_pwm_isr())
    {
        power_save_pwm_isr(param);
        GLOBAL_INT_RESTORE();
        return;
    }
#endif
	current_clock ++;

    #if CFG_BK7221_MDM_WATCHDOG_PATCH
    rc_reset_patch();
    #endif

    rt_tick_increase();
	GLOBAL_INT_RESTORE();

	if (--second_countdown == 0) 
	{
		current_seconds ++;
		second_countdown = FCLK_SECOND;

        #if defined(RT_USING_ALARM)
        rt_alarm_update(NULL, 0);
        #endif
	}
}

static UINT32 fclk_freertos_update_tick(UINT32 tick)
{
    current_clock += tick;

    while(tick >= FCLK_SECOND)
    {
        current_seconds ++;
        tick -= FCLK_SECOND;
    }

    if(second_countdown <= tick)
    {
        current_seconds ++;
        second_countdown = FCLK_SECOND - (tick - second_countdown);
    }
    else
    {
        second_countdown -= tick;
    }

    return 0;
}

#if (CFG_SUPPORT_RTT)
UINT32 rtt_update_tick(UINT32 tick)
{
    if (tick)
    {
        rt_enter_critical();

        /* adjust OS tick */
        rt_tick_set(rt_tick_get() + tick);
        /* check system timer */
        rt_timer_check();
        
        rt_exit_critical();
    }
}
#endif

UINT32 fclk_update_tick(UINT32 tick)
{
#if (CFG_SUPPORT_RTT)
    rtt_update_tick(tick);
#elif (CFG_SUPPORT_ALIOS)
    krhino_update_sys_tick((UINT64)tick);
#else
    GLOBAL_INT_DECLARATION();

    if(tick == 0)
        return 0;

    GLOBAL_INT_DISABLE();
    fclk_freertos_update_tick(tick);
    vTaskStepTick( tick );
    GLOBAL_INT_RESTORE();
    
#endif
    return 0;
}

UINT32 fclk_freertos_get_tick(void)
{
    return current_clock;
}

UINT64 fclk_get_tick(void)
{
    UINT64 fclk;
#if (CFG_SUPPORT_RTT)
    fclk = (UINT64)rt_tick_get();
#elif (CFG_SUPPORT_ALIOS)
    fclk = krhino_sys_tick_get();
#else
    fclk = (UINT64)fclk_freertos_get_tick();
#endif
    return fclk;
}

UINT32 fclk_get_second(void)
{
#if (CFG_SUPPORT_RTT)
    return (rt_tick_get()/FCLK_SECOND);
#elif (CFG_SUPPORT_ALIOS)
    return (krhino_sys_tick_get()/FCLK_SECOND);
#else
    return current_clock/FCLK_SECOND;
#endif
}

UINT32 fclk_from_sec_to_tick(UINT32 sec)
{
    return sec * FCLK_SECOND;
}

void fclk_reset_count(void)
{
    current_clock = 0;
    current_seconds = 0;
}

UINT32 fclk_cal_endvalue(UINT32 mode)
{
    UINT32 value = 1;

    if(PWM_CLK_32K == mode)
    {
        /*32k clock*/
        value = FCLK_DURATION_MS * 32;
    }
    else if(PWM_CLK_26M == mode)
    {
        /*26m clock*/
#if CFG_SUPPORT_ALIOS
	value = 26000000 / RHINO_CONFIG_TICKS_PER_SECOND;
#else
        value = FCLK_DURATION_MS * 26000;
#endif
    }

    return value;
}

void os_clk_init(void)
{
    UINT32 ret;
    pwm_param_t param;

    /*init pwm*/
    param.channel         = FCLK_PWM_ID;
    param.cfg.bits.en     = PWM_ENABLE;
    param.cfg.bits.int_en = PWM_INT_EN;
    param.cfg.bits.mode   = PMODE_TIMER;

#if(CFG_RUNNING_PLATFORM == FPGA_PLATFORM)  // FPGA:PWM0-2-32kCLK, pwm3-5-24CLK
    param.cfg.bits.clk    = PWM_CLK_32K;
#else
    param.cfg.bits.clk    = PWM_CLK_26M;
#endif

    param.p_Int_Handler   = fclk_hdl;
    param.duty_cycle      = 0;
    param.end_value       = fclk_cal_endvalue((UINT32)param.cfg.bits.clk);

    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_INIT_PARAM, &param);
    ASSERT(PWM_SUCCESS == ret);
}

// eof

