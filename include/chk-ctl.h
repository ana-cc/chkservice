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

#ifndef _CHK_CTL_H
#define _CHK_CTL_H

#include "chk-systemd.h"

typedef struct UnitItem {
  std::string id;
  std::string target;
  std::string description;
  int sub;
  int state;
} UnitItem;

enum {
  UNIT_STATE_DISABLED,
  UNIT_STATE_ENABLED,
  UNIT_STATE_STATIC,
  UNIT_STATE_BAD,
  UNIT_STATE_MASKED
};

enum {
  UNIT_SUBSTATE_RUNNING,
  UNIT_SUBSTATE_CONNECTED,
  UNIT_SUBSTATE_INVALID
};

class ChkCTL {
  public:
    ChkCTL();
    ~ChkCTL();
    ChkBus *bus;
    std::vector<UnitItem *> getItemsSorted();
    std::vector<UnitItem *> getByTarget(const char *target);
    std::vector<UnitItem *> getItems();
    void toggleUnitState(UnitItem *item);
    void toggleUnitSubState(UnitItem *item);
    void fetch();
  private:
    std::vector<UnitItem *> items;
    void pushItem(UnitInfo *unit);
    void sortByName(std::vector<UnitItem *> *sortable);
};

#endif
