#ifndef _CHK_CTL_H
#define _CHK_CTL_H

#include "chk-systemd.h"

typedef struct UnitItem {
  UnitInfo *sysUnit;
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
    std::vector<UnitItem *> getItemsSorted();
    std::vector<UnitItem *> getByTarget(const char *target);
    std::vector<UnitItem *> getItems();
    void updateItems();
  private:
    ChkBus *bus;
    std::vector<UnitItem *> items;
    void fetch();
    void pushItem(UnitInfo unit);
    void sortByName(std::vector<UnitItem *> *sortable);
};

#endif
