#define PK_SCHEDULED_LAUNCH 271538945
#define SCHEDULED_LAUNCH_COOKIE 0x5a4d

#if days_of_week
inline void %hook(startup)(void) {
  if (wakeup_schedule == 0) return;
  WakeupId wk_id = persist_read_int(PK_SCHEDULED_LAUNCH);
   if (wk_id) {
    wakeup_cancel(wk_id);
   }

   time_t now = time(NULL);
   struct tm * now_cmps = localtime(&now);

   now_cmps->tm_sec = 0;
   now_cmps->tm_min = wakeup_minutes;
   now_cmps->tm_hour = wakeup_hours;
   int original_dom = now_cmps->tm_mday;
   time_t schedule_time;

   for (int i = 0; i < 8; ++i) {
    if (!(days_of_week & (1 << (now_cmps->tm_wday + i) % 7))) continue;
      now_cmps->tm_mday = original_dom + i;
      schedule_time = mktime(now_cmps);
      if (schedule_time > now) break;
   }
   if (schedule_time <= now) return; // Who knows, maybe they didn't schedule anything?

   do {
     wk_id = wakeup_schedule(schedule_time, SCHEDULED_LAUNCH_COOKIE, false);
     schedule_time += 60;
   } while (wk_id == E_RANGE);

   if (wk_id >= 0) persist_write_int(PK_SCHEDULED_LAUNCH, wk_id);
}
#endif
