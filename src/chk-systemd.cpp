#include <iostream>
#include <vector>
#include <cassert>
#include <set>

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

  return status < 0 ?  NULL : strdup(state);
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
    disconnect();

    if (status < 0) {
      throw std::string(errorMessage);
    }

  return units;
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

void ChkBus::callUnit(const char *method, char **names, bool force) {
  int status;

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
    setErrorMessage(error.message);
    goto finish;
  }

  status = sd_bus_message_append_strv(busMessage, names);

  if (status < 0) {
    setErrorMessage(status);
    goto finish;
  }

  if (force) {
    status = sd_bus_message_append(busMessage, "bb", true, true);
  } else {
    status = sd_bus_message_append(busMessage, "b", true);
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

  std::cout << method << "ll is ok..." << std::endl;
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
  char *names[ids->size()];

  for (auto id : (*ids)) {
    names[i] = (char *) id.c_str();
    i++;
  }
  names[i] = NULL;

  callUnit("EnableUnitFiles", names, true);
}

void ChkBus::disableUnits(std::set<std::string> *ids) {
  int i = 0;
  char *names[ids->size()];

  for (auto id : (*ids)) {
    names[i] = (char *) id.c_str();
    i++;
  }
  names[i] = NULL;

  callUnit("DisableUnitFiles", names, false);
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
