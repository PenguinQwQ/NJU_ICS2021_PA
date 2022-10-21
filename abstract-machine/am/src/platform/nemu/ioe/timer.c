#include <am.h>
#include <nemu.h>
#include <klib.h>

static uint64_t boot_time = 0;
void __am_timer_init() {
  boot_time = ((uint64_t)inl(RTC_ADDR + 4));// + (((uint64_t)inl(RTC_ADDR)) << 32);
//  printf("boot time is:%d\n", &boot_time);
  return;
}
void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uint64_t now_time =(uint64_t)inl(RTC_ADDR + 4);//+ (((uint64_t)inl(RTC_ADDR)) << 32);
//  printf("now time is:%d\n", &now_time);
  uptime->us = now_time - boot_time;
//  printf("us is %d\n", uptime->us);
}
 
void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
