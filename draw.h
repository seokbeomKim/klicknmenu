#ifndef _DRAW_H_
#define _DRAW_H_

/* DpyContent: container that has every variables which needs to represent menu. */
typedef struct DpyContent DpyContent;
typedef struct DpyContent {
	Display *dpy;

        /* Window spsecifications */
	int x, y, width, height;
	Window        win;
	int           screen;
	int           depth;
	GC            gc;
	Font          font;
	XFontStruct  *fontinfo;     

        /* entry head node that menu will have */
	ENTRY* first_entry;

	DpyContent* next;
	DpyContent* prev;
} DpyContent;

typedef struct {
	int x;
	int y;
} mouse_pos;

ENTRY* current_entry;

int           openDisplay(DpyContent* display);
int           closeDisplay(DpyContent* display);
int           getEntryCount(ENTRY* entry);
void          initdraw(DpyContent* display);
int           alignPosition(DpyContent* display, char* str);
int           draw(void);
void          drawEntry(ENTRY* entry, DpyContent* display, ENTRY* current);
void          drawSubmenu(ENTRY* parent, DpyContent* display, Window window);
void          freedraw(DpyContent* display);
void          destroyWindow( DpyContent* display );
void          getMice(mouse_pos* pos, DpyContent* display);
DpyContent*   newDpyContent(DpyContent* dpy, int x, int y, int width, int height);
void          OpenWindow(DpyContent* display, Window parent);
unsigned long getColor(DpyContent* display, char* colorstr);
#endif
