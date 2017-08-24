#include <iostream>
#include <vector>
#include <cassert>

#include "chk-systemd.h"

ChkBus::ChkBus() {

}

ChkBus::~ChkBus() {
  disconnect();
}

bool ChkBus::connect() {
  int status;
  errorMessage.clear();

  if (isConnected()) {
    disconnect();
  }

  status = sd_bus_open_system(&bus);

  if (status < 0) {
    setErrorMessage(status);
    disconnect();

    throw std::string(errorMessage);
  }

  return isConnected();
}

void ChkBus::disconnect() {
  if (bus != NULL) {
    sd_bus_unref(bus);
    bus = NULL;
  }
}

bool ChkBus::isConnected() {
  return bus == NULL ? false : true;
}

void ChkBus::setErrorMessage(int status) {
  errorMessage = (char *)ERR_PREFIX;

  if (status < 0) {
    status = -status;
  }

  errorMessage += strerror(status);
}

void ChkBus::setErrorMessage(const char *message) {
  errorMessage = (char *)ERR_PREFIX;
  errorMessage += message;
}

const char *ChkBus::getState(const char *name) {
  int status;
  const char *state;

  errorMessage.clear();

  if (!isConnected()) {
    connect();
  }

  sd_bus_message *busMessage = NULL;
  sd_bus_error error = SD_BUS_ERROR_NULL;

  status = sd_bus_call_method(
    bus,
    "org.freedesktop.systemd1",
    "/org/freedesktop/systemd1",
    "org.freedesktop.systemd1.Manager",
    "GetUnitFileState",
    &error,
    &busMessage,
    "s",
    name);

  if (status < 0) {
    setErrorMessage(error.message);
    goto finish;
  }

  status = sd_bus_message_read(busMessage, "s", &state);

  if (status < 0) {
    setErrorMessage(status);
    goto finish;
  }

  finish:
    sd_bus_error_free(&error);
    sd_bus_message_unref(busMessage);
    if (state < 0) {
      throw std::string(errorMessage);
    }

  return status < 0 ? NULL : strdup(state);
}

std::vector<UnitInfo> ChkBus::getUnitFiles() {
  int status;
  const char *state;
  char *path;
  std::vector<UnitInfo> units;

  sd_bus_message* busMessage = NULL;
  sd_bus_message* reply = NULL;
  sd_bus_error error = SD_BUS_ERROR_NULL;

  errorMessage.clear();

  if (!isConnected()) {
    connect();
  }

  status = sd_bus_message_new_method_call(
    bus,
    &busMessage,
    "org.freedesktop.systemd1",
    "/org/freedesktop/systemd1",
    "org.freedesktop.systemd1.Manager",
    "ListUnitFiles");

  if (status < 0) {
    setErrorMessage(status);
    goto finish;
  }

  status = sd_bus_call(bus, busMessage, 0, &error, &reply);

  if (status < 0) {
    setErrorMessage(error.message);
    goto finish;
  }

  status = sd_bus_message_enter_container(reply, SD_BUS_TYPE_ARRAY, "(ss)");

  if (status < 0) {
    setErrorMessage(status);
    goto finish;
  }

  while ((status = sd_bus_message_read(reply, "(ss)", &path, &state)) > 0) {
    UnitInfo *unit = new UnitInfo();

    unit->unitPath = strdup(path);
    unit->state = strdup(state);

    std::string p = std::string(strdup(unit->unitPath));
    unit->id = strdup(p.substr(p.find_last_of("/") + 1, p.length()).c_str());

    units.push_back((*unit));
  }

  status = sd_bus_message_exit_container(reply);

  if (status < 0) {
    setErrorMessage(status);
    goto finish;
  }

  finish:
    sd_bus_error_free(&error);
    sd_bus_message_unref(busMessage);
    sd_bus_message_unref(reply);
    disconnect();

    if (status < 0) {
      throw std::string(errorMessage);
    }

  for (std::vector<UnitInfo>::iterator iter = units.begin(); iter != units.end(); iter++) {
  }

  return units;
}

std::vector<UnitInfo> ChkBus::getUnits() {
  int status;
  UnitInfo unit;
  std::vector<UnitInfo> units;

  sd_bus_message* busMessage = NULL;
  sd_bus_message* reply = NULL;
  sd_bus_error error = SD_BUS_ERROR_NULL;

  errorMessage.clear();

  if (!isConnected()) {
    connect();
  }

  status = sd_bus_message_new_method_call(
    bus,
    &busMessage,
    "org.freedesktop.systemd1",
    "/org/freedesktop/systemd1",
    "org.freedesktop.systemd1.Manager",
    "ListUnits"
  );

  if (status < 0) {
    setErrorMessage(status);
    goto finish;
  }

  status = sd_bus_call(bus, busMessage, 0, &error, &reply);

  if (status < 0) {
    setErrorMessage(error.message);
    goto finish;
  }

  status = sd_bus_message_enter_container(reply, SD_BUS_TYPE_ARRAY, "(ssssssouso)");

  if (status < 0) {
    setErrorMessage(status);
    goto finish;
  }

  while ((status = busParseUnit(reply, &unit)) > 0) {
    try {
      unit.state = getState(unit.id);
    } catch (std::string &err) {
      break;
    }
    units.push_back(unit);
  }

  status = sd_bus_message_exit_container(reply);

  if (status < 0) {
    setErrorMessage(status);
    goto finish;
  }

  finish:
    sd_bus_error_free(&error);
    sd_bus_message_unref(busMessage);
    sd_bus_message_unref(reply);
    disconnect();

    if (status < 0) {
      throw std::string(errorMessage);
    }

  return units;
}

std::vector<UnitInfo> ChkBus::getAllUnits() {
  std::vector<UnitInfo> files = getUnitFiles();
  std::vector<UnitInfo> units = getUnits();

  for (auto unit : units) {
    int idx = 0;
    bool found = false;
    for (auto file : files) {
      std::string uid(unit.id);
      std::string fid(file.id);

      if (uid.find(fid) == 0) {
        files[idx].description = strdup(unit.description);
        files[idx].loadState = strdup(unit.loadState);
        files[idx].activeState = strdup(unit.activeState);
        files[idx].subState = strdup(unit.subState);
        files[idx].following = strdup(unit.following);
        files[idx].jobId = unit.jobId;
        files[idx].jobType = strdup(unit.jobType);
        files[idx].jobPath = strdup(unit.jobPath);
        if (unit.state) {
          files[idx].state = strdup(unit.state);
        }
        found = true;
      }
      idx++;
    }

    if (!found) {
      files.push_back(unit);
    }
  }

  units.clear();

  return files;
}

void ChkBus::reloadDaemon() {
  int status;

  sd_bus_message* busMessage = NULL;
  sd_bus_error error = SD_BUS_ERROR_NULL;

  if (!isConnected()) {
    connect();
  }

  status  = sd_bus_message_new_method_call(
    bus,
    &busMessage,
    "org.freedesktop.systemd1",
    "/org/freedesktop/systemd1",
    "org.freedesktop.systemd1.Manager",
    "Reload"
  );

  if (status < 0) {
     setErrorMessage(status);
     goto finish;
  }

  sd_bus_call(bus, busMessage, 1000, &error, NULL);

  finish:
    sd_bus_error_free(&error);
    sd_bus_message_unref(busMessage);
    disconnect();

    if (status < 0) {
      throw std::string(errorMessage);
    }
}

void ChkBus::applyUnitState(const char *method, char **names, int flags) {
  int status;
  bool checkState = false;

  sd_bus_error error = SD_BUS_ERROR_NULL;
  sd_bus_message *busMessage = NULL;

  if (!isConnected()) {
    connect();
  }

  status = sd_bus_message_new_method_call(
    bus,
    &busMessage,
    "org.freedesktop.systemd1",
    "/org/freedesktop/systemd1",
    "org.freedesktop.systemd1.Manager",
    method);

  if (status < 0) {
    setErrorMessage(status);
    goto finish;
  }

  status = sd_bus_message_append_strv(busMessage, names);

  if (status < 0) {
    setErrorMessage(status);
    goto finish;
  }

  switch(flags) {
    case STATE_FLAGS_ENABLE:
      status = sd_bus_message_append(busMessage, "bb", true, true);
      break;
    case STATE_FLAGS_DISABLE:
      status = sd_bus_message_append(busMessage, "b", true);
      checkState = true;
      break;
    case STATE_FLAGS_DISABLE_ISO:
      status = sd_bus_message_append(busMessage, "b", false);
      checkState = true;
      break;
    default:
      break;
  }

  if (status < 0) {
    setErrorMessage(status);
    goto finish;
  }

  status = sd_bus_call(bus, busMessage, 0, &error, NULL);

  if (status < 0) {
    setErrorMessage(error.message);
    goto finish;
  }

  finish:
    sd_bus_error_free(&error);
    sd_bus_message_unref(busMessage);
    disconnect();

    if (status < 0) {
      throw std::string(errorMessage);
    }

  if (checkState) {
    checkDisabledStatus(names);
  }
}

void ChkBus::checkDisabledStatus(char **names) {
  for (int i = 0; names[i] != NULL; i++) {
    const char *name = names[i];
    const char *state = getState(name);

    if (std::string(state).find("enabled") == 0) {
      applyUnitState("DisableUnitFiles", names, STATE_FLAGS_DISABLE_ISO);
      applyUnitState("DisableUnitFiles", names, STATE_FLAGS_DISABLE);
    }

    if (state != NULL) {
      free((void *)state);
    }
  }
}

void ChkBus::applyUnitSub(const char *name, const char *method) {
  int status;

  sd_bus_error error = SD_BUS_ERROR_NULL;
  sd_bus_message *busMessage = NULL;
  sd_bus_message *reply = NULL;

  if (!isConnected()) {
    connect();
  }

  status = sd_bus_call_method(
    bus,
    "org.freedesktop.systemd1",
    "/org/freedesktop/systemd1",
    "org.freedesktop.systemd1.Manager",
    method,
    &error,
    &reply,
    "ss",
    name,
    "replace-irreversibly");

  if (status < 0) {
    setErrorMessage(error.message);
    goto finish;
  }

  finish:
    sd_bus_error_free(&error);
    sd_bus_message_unref(busMessage);
    disconnect();

    if (status < 0) {
      throw std::string(errorMessage);
    }
}

void ChkBus::enableUnits(std::set<std::string> *ids) {
  int i = 0;

  if (ids->size() < 1) {
    return;
  }

  char *names[ids->size()];

  for (auto id : (*ids)) {
    names[i] = (char *) id.c_str();
    i++;
  }
  names[i] = NULL;

  applyUnitState("EnableUnitFiles", names, STATE_FLAGS_ENABLE);
}

void ChkBus::disableUnits(std::set<std::string> *ids) {
  int i = 0;

  if (ids->size() < 1) {
    return;
  }

  char *names[ids->size()];

  for (auto id : (*ids)) {
    names[i] = (char *) id.c_str();
    i++;
  }

  names[i] = NULL;

  applyUnitState("DisableUnitFiles", names, STATE_FLAGS_DISABLE);
}

void ChkBus::enableUnit(const char *name) {
  try {
    std::set<std::string> id;
    id.insert(name);
    enableUnits(&id);
  } catch (std::string &err) {
    throw err;
  }
}

void ChkBus::disableUnit(const char *name) {
  try {
    std::set<std::string> id;
    id.insert(name);
    disableUnits(&id);
  } catch (std::string &err) {
    throw err;
  }
}

void ChkBus::stopUnits(std::set<std::string> *ids) {
  for (auto id : (*ids)) {
    applyUnitSub(id.c_str(), "StopUnit");
  }
}

void ChkBus::startUnits(std::set<std::string> *ids) {
  for (auto id : (*ids)) {
    applyUnitSub(id.c_str(), "StartUnit");
  }
}

void ChkBus::stopUnit(const char *name) {
  applyUnitSub(name, "StopUnit");
}

void ChkBus::startUnit(const char *name) {
  applyUnitSub(name, "StartUnit");
}

