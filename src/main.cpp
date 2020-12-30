#include <esp32/rom/ets_sys.h>
#include <stdlib.h>
#include <string.h>

int main();
void rtc_wdt_feed(void);
void clear_cache();
extern "C" {
//  int ets_printf(const char *fmt, ...); /* from ROM */
  extern int _bss_start;
  extern int _bss_end;
  extern int _rtc_bss_start;
  extern int _rtc_bss_end;
  extern int _init_start;
}
extern "C" void esp_config_instruction_cache_mode(void);

/* CPU0 entry point (PRO CPU) */
extern "C" void __attribute__((noreturn)) call_start_cpu0(void){
  //cpu_hal_set_vecbase(&_init_start);
  memset(&_bss_start, 0, (&_bss_end - &_bss_start) * sizeof(_bss_start));
 // esp_config_instruction_cache_mode();
  ets_install_uart_printf();
  (void) main();
}

/* CPU1 entry point (APP CPU) */
extern "C" void __attribute__((noreturn)) call_start_cpu1(void){
  while(1) {}
}
/* IRAM_ATTR for interrupt functions */

int __attribute__((noreturn)) main() {
  ets_printf("Hello, world!\n");
  clear_cache();
  while(1) {
    /* reset all watchdogs so that we don't crash */
    rtc_wdt_feed();
    //reset watchdog of timer group 0, timer 0
    //TIMGn_Tx_WDTFEED_REG, write any value to reset 
    *(uint32_t*)(0x3FF5F000 + 0x0060) = 0xAABBCCDD;
    ets_printf("Looping!\n");
    ets_delay_us(1000 * 1000);
  }
}

#include <soc/rtc_cntl_reg.h>

#define REG_SET_BITS(_r, _b, _m) ({                                                                                    \
            ASSERT_IF_DPORT_REG((_r), REG_SET_BITS);                                                                   \
            (*(volatile uint32_t*)(_r) = (*(volatile uint32_t*)(_r) & ~(_m)) | ((_b) & (_m)));                         \
        })

bool rtc_wdt_get_protect_status(void)
{
    return READ_PERI_REG(RTC_CNTL_WDTWPROTECT_REG) != RTC_CNTL_WDT_WKEY_VALUE;
}

void rtc_wdt_protect_off(void)
{
    WRITE_PERI_REG(RTC_CNTL_WDTWPROTECT_REG, RTC_CNTL_WDT_WKEY_VALUE);
}

void rtc_wdt_protect_on(void)
{
    WRITE_PERI_REG(RTC_CNTL_WDTWPROTECT_REG, 0);
}

void rtc_wdt_feed(void)
{
    bool protect = rtc_wdt_get_protect_status();
    if (protect) {
        rtc_wdt_protect_off();
    }

    REG_SET_BIT(RTC_CNTL_WDTFEED_REG, RTC_CNTL_WDT_FEED);
    if (protect) {
        rtc_wdt_protect_on();
    }

}
//#include <soc/dport_access.h>
#define _DPORT_REG_READ(_r)        (*(volatile uint32_t *)(_r))
#define _DPORT_REG_WRITE(_r, _v)   (*(volatile uint32_t *)(_r)) = (_v)
#include <soc/dport_reg.h>

void clear_cache() {
  _DPORT_REG_WRITE(DPORT_PRO_CACHE_CTRL1_REG,
            _DPORT_REG_READ(DPORT_PRO_CACHE_CTRL1_REG) | DPORT_PRO_CACHE_MMU_IA_CLR);
    _DPORT_REG_WRITE(DPORT_PRO_CACHE_CTRL1_REG,
            _DPORT_REG_READ(DPORT_PRO_CACHE_CTRL1_REG) & (~DPORT_PRO_CACHE_MMU_IA_CLR));
}

