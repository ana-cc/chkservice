#ifndef _CHK_UI_H
#define _CHKUI_H

#include <curses.h>
#include "chk-ctl.h"

typedef struct RECTANGLE {
  int x;
  int y;
  int w;
  int h;
} RECTANGLE;

class MainWindow {
  public:
    static void sigwinch(int c);
    WINDOW *win;
    MainWindow();
    ~MainWindow();
    void createMenu();
  private:
    RECTANGLE *screenSize = new RECTANGLE();
    RECTANGLE *winSize = new RECTANGLE();
    RECTANGLE *padding = new RECTANGLE();
    ChkCTL *ctl = new ChkCTL;
    std::vector<UnitItem *> units;
    int selected = 0;
    int start = 0;
    void resize();
    void moveUp();
    void moveDown();
    void movePageUp();
    void movePageDown();
    void drawUnits();
    void drawItem(UnitItem *unit, int y);
    void drawInfo();
    void toggleUnitState();
    void toggleUnitSubState();
    void error(char *err);
};

void startCurses();
void stopCurses();
void printInMiddle(WINDOW *win, int starty, int startx, int width,
    char *string, chtype color, char *sp);
void aboutWindow(RECTANGLE *parent);

#endif
