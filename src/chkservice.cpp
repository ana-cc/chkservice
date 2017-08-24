#include <iostream>

#include "chk-systemd.h"
#include "chk-ctl.h"
#include "chk-ui.h"
#include "chk.h"

using namespace std;

int main(int ac, char **av) {
  if (ac > 1) {
    fprintf(stdout, ABOUT_INFO, VERSION);
    return 0;
  }

  startCurses();

  MainWindow *mainWindow = new MainWindow();
  mainWindow->createMenu();

  delete mainWindow;
  stopCurses();

  return 0;
}
