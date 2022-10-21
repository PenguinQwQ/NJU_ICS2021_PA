#include <am.h>
#include <nemu.h>
#include <klib.h>

static uint32_t boot_time = 0;
void __am_timer_init() {
  boot_time = inl(RTC_ADDR);
  printf("boot time is:%d\n", &boot_time);
  return;
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uint32_t now_time = inl(RTC_ADDR);
//  printf("now time is:%d\n", &now_time);
  uptime->us = now_time - boot_time;
}
 
void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
