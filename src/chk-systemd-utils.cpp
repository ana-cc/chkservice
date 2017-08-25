/*
 *    chkservice is a tool for managing systemd units.
 *    more infomration at https://github.com/linuxenko/chkservice
 *
 *    Copyright (C) 2017 Svetlana Linuxenko <svetlana(at)linuxenko.pro>
 *
 *    chkservice program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    chkservice program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
