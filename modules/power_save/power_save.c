#include <pebble.h>

#ifdef coarse_tick_timers

void tick_timer_service_subscribe__patch(TimeUnits units, TickHandler handler) {
  if (units == SECOND_UNIT) units = MINUTE_UNIT;
  tick_timer_service_subscribe(units, handler);
}

#endif

#ifdef disable_appmessage
// Since nobody actually checks the results of these ever, we can't just return an error on app_message_open and assume they'll give up

AppMessageResult app_message_open__patch(const uint32_t size_inbound, const uint32_t size_outbound) {
  return APP_MSG_OK;
}

AppMessageResult app_message_outbox_begin__patch(DictionaryIterator** iter) {
  static char massive_buffer;
  dict_write_begin(*iter, &massive_buffer, 1);
  return APP_MSG_OK;
}

AppMessageResult app_message_outbox_send__patch(void) {
  return APP_MSG_OK;
}

#endif
