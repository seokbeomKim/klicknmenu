#ifndef _KLICKNMENU_H_
#define _KLICKNMENU_H_

#define COPY_INTVALUE(v) \
                v = atoi(pch);

#define COPY_STRVALUE(v) {                                              \
		char *temp = malloc(sizeof(char) * (strlen(pch) + 1));	\
                strncpy( temp, pch, strlen(pch) );                      \
                v = temp;                                               \
                continue;                                               \
        }
#define ISOPTION(v)  ( strncmp(pch, v, strlen(v)) == 0 )
#define ISENDMENU(v) ( strncmp(v, OPTION_ENDMENU, strlen(OPTION_ENDMENU)) == 0 )

/* miscelleneous values for parsing config file  */
extern int    menu_width;
extern int    menu_height;
extern int    menu_padding;
extern int    border_width;
extern char*  border_color;
extern int    entry_height;
extern char*  bgColor;
extern char*  fgColor;
extern char*  focus_bgColor;
extern char*  focus_fgColor;
extern char*  fontname;

/* location values of mouse */
extern int    pointer_x;
extern int    pointer_y;
extern ALIGN  align;
extern ENTRY* lastentry;
extern ENTRY* menu;

void   initmenu(void);
void   checkValues(void);
int    getLevel(char* str);
int    numberOfMenu(int level);
ENTRY* makeNewEntry(char* str);

void   initConfig(FILE* fp);
char*  trimTabspace(char* str);
void   tolowerString(char* str);

#ifdef _DEBUG_ON_
void   showEntry(ENTRY* target);
void   showMenu(ENTRY* ent);
void   showValues(void);
#endif

#endif
