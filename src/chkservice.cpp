#include <iostream>

#include "chk-systemd.h"
#include "chk-ctl.h"
#include "chk-ui.h"

using namespace std;

int main(int ac, char **av) {
  startCurses();

  MainWindow *mainWindow = new MainWindow();
  mainWindow->createMenu();

  delete mainWindow;
  stopCurses();

  return 0;
}
