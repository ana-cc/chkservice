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
#include <unistd.h>
#include <sys/wait.h>

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
    NULL,
    &u->unitPath,
    NULL,
    NULL,
    NULL);
}

void applySYSv(const char *state, const char **names) {
  int pid = fork();
  int status;

  if (pid == 0) {
    for (int i = 0; names[i] != NULL; i++) {
      std::string unitName(names[i]);
      std::string sysvCMD = SYSV_INSTALL_EXEC;

      unitName = unitName.substr(0, unitName.find_last_of('.'));
      sysvCMD += " ";
      sysvCMD += state;
      sysvCMD += " ";
      sysvCMD += unitName;
      sysvCMD += " > /dev/null 2>&1";

      if ((status = system(sysvCMD.c_str())) == -1) {
        break;
      }
    }
    exit(status);
  } else {
    waitpid(pid, NULL, 0);
  }
}
