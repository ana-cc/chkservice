#include "chk-systemd.h"
#include <cassert>

int busParseUnit(sd_bus_message *message, UnitInfo *u) {
  assert(message);
  assert(u);

  return sd_bus_message_read(
    message,
    "(ssssssouso)",
    &u->id,
    &u->description,
    &u->loadState,
    &u->activeState,
    &u->subState,
    &u->following,
    &u->unitPath,
    &u->jobId,
    &u->jobType,
    &u->jobPath);
}
