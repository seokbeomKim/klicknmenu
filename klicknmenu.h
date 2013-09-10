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
int    menu_width;
int    menu_height;
int    menu_padding;
int    border_width;
char*  border_color;
int    entry_height;
char*  bgColor;
char*  fgColor;
char*  focus_bgColor;
char*  focus_fgColor;
char*  fontname;

/* location values of mouse */
int    pointer_x;
int    pointer_y;
ALIGN  align;
ENTRY* lastentry;
ENTRY* menu;

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
