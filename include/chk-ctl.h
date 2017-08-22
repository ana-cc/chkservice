#ifndef _CHK_CTL_H
#define _CHK_CTL_H

#include "chk-systemd.h"

typedef struct UnitItem {
  UnitInfo sysUnit;
  std::string name;
  std::string target;
} UnitItem;

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
