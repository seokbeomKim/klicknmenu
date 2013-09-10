#ifndef _MENU_ENTRY_
#define _MENU_ENTRY_

/* ************DEBUG FLAGS************* */
/* #define _DEBUG_ON_	*/
/* ************************************ */

typedef struct ENTRY ENTRY;
typedef struct ENTRY {
	char*   name;
	char*   command;
	ENTRY*  child;
	ENTRY*  parent;
	char*   xpmPath;
	ENTRY*  prev;
	ENTRY*  next;
	int     level;
} ENTRY;

typedef enum {
	LEFT = 0,
	CENTER,
	RIGHT
} ALIGN;

typedef enum {
	false = 0,
	true
} bool;

/* value for parsing data */
#define TOKEN_VALUES           "\n \"\'"
#define TOKEN_FONTNAME         "\"\'\n"
#define TOKEN_MENU             "\t \"\'"
#define DEFAULT_ALIGN          LEFT
#define OPTION_FONTNAME        "font_name"
#define OPTION_MENUWIDTH       "menu_width"
#define OPTION_MENUHEIGHT      "menu_height"
#define OPTION_MENUPADDING     "menu_padding"
#define OPTION_BORDERWIDTH     "border_width"
#define OPTION_BORDERCOLOR     "border_color"
#define OPTION_BACKGROUND      "background"
#define OPTION_FOREGROUND      "foreground"
#define OPTION_ENTRYHEIGHT     "entry_height"
#define OPTION_TEXTALIGN       "text_align"
#define OPTION_FOCUSBACKGROUND "focus_background"
#define OPTION_FOCUSFOREGROUND "focus_foreground"
#define OPTION_MENU            "menu"
#define OPTION_ENDMENU         "endmenu"
#define BUFSIZE                128

/* default values for global value */
#define DEFAULT_FONTNAME      "fixed"
#define DEFAULT_MENUWIDTH     150
#define DEFAULT_MENUHEIGHT    150
#define DEFAULT_MENUPADDING   5
#define DEFAULT_BORDERWIDTH   0
#define DEFAULT_BORDERCOLOR   "#ffffff"
#define DEFAULT_FOREGROUND    "#ffffff"
#define DEFAULT_BACKGROUND    "#000000"
#define DEFAULT_ENTRYHEIGHT   20
#define DEFAULT_TEXTALIGN     "LEFT"
#define DEFAULT_FOCUSBACKGROUND "#000000"
#define DEFAULT_FOCUSFOREGROUND "#6dbcdb"

#endif
