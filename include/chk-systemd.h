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

#ifndef _CHK_SYSTEMD_H
#define _CHK_SYSTEMD_H

#include <iostream>
#include <set>
#include <vector>
#include <systemd/sd-bus.h>

#define ERR_PREFIX "Failed: "

enum STATE_FLAGS {
  STATE_FLAGS_ENABLE,
  STATE_FLAGS_DISABLE,
  STATE_FLAGS_DISABLE_ISO
};

typedef struct UnitInfo {
  const char *id;
  const char *description;
  const char *loadState;
  const char *activeState;
  const char *subState;
  const char *following;
  const char *unitPath;
  uint32_t jobId;
  const char *jobType;
  const char *jobPath;
  const char *state;
} UnitInfo;

class ChkBus {
  public:
    ChkBus();
    ~ChkBus();

    bool connect();
    void disconnect();
    bool isConnected();

    void setErrorMessage(int status);
    void setErrorMessage(const char *message);

    std::vector<UnitInfo> getUnits();
    std::vector<UnitInfo> getUnitFiles();
    std::vector<UnitInfo> getAllUnits();

    void disableUnit(const char *name);
    void enableUnit(const char *name);
    void disableUnits(std::set<std::string> *ids);
    void enableUnits(std::set<std::string> *ids);

    void startUnit(const char *name);
    void stopUnit(const char *name);
    void startUnits(std::set<std::string> *ids);
    void stopUnits(std::set<std::string> *ids);

    void reloadDaemon();

  private:
    sd_bus* bus = NULL;
    std::string errorMessage;
    const char* getState(const char *name);
    void applyUnitState(const char *method, char **names, int flags);
    void applyUnitSub(const char *name, const char *method);
    void checkDisabledStatus(char **names);
};

int busParseUnit(sd_bus_message *message, UnitInfo *u);

#endif
