#ifndef _CHK_SYSTEMD_H
#define _CHK_SYSTEMD_H

#include <iostream>
#include <vector>
#include <systemd/sd-bus.h>

#define ERR_PREFIX "Failed: "

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

    void disableUnit(const char *name);
    void enableUnit(const char *name);

    void reloadDaemon();

  private:
    sd_bus* bus = NULL;
    std::string errorMessage;
    const char* getState(const char *name);
    void callUnit(const char *method, const char *name, bool force);
};

int busParseUnit(sd_bus_message *message, UnitInfo *u);

#endif
