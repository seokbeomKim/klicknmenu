#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <assert.h>
#include "types.h"
#include "klicknmenu.h"
#include "draw.h"

/* initmenu: with $HOME environment value, make the configuration file path 
   and read the file, starting to make entry nodes. */
extern void
initmenu(void)
{
	char*   homedir;
	char*   configPath;
        FILE*   fp;
	
        /* initialize menu value as NULL */
	menu    = NULL;		
	homedir = getenv("HOME");
	if( homedir == NULL ) {
		fprintf(stderr, "$HOME environment value is not set.\n");
		exit(1);
	}
        
        /* make the config path */
	configPath = (char *) malloc( sizeof(char) * ( strlen(homedir) + strlen("/.klicknmenu")) + 1);

	strncpy( configPath, homedir, strlen( homedir ) );
	strncat( configPath, "/.klicknmenu", strlen("/.klicknmenu") );
	
	if ( (fp = fopen( configPath, "r" )) == NULL )
        {
		fprintf(stderr, "configuration file isn't exist: %s\n", configPath );
		exit(2);
	} 
        else
        {
                /* if the file exist, make the entry nodes. */
		initConfig(fp);
	}
	
        fclose(fp);
}

/* checkValue: if configuration file has not proper values, alternatively, set the default value. */
extern void
checkValues(void)
{
	if( menu_width   < 0 )     menu_width  = DEFAULT_MENUWIDTH;
	if( menu_height  < 0 )     menu_height = DEFAULT_MENUHEIGHT;
	if( menu_padding < 0 )     menu_padding = DEFAULT_MENUPADDING;
	if( border_width < 0 )     border_width = DEFAULT_BORDERWIDTH;
	if( entry_height < 0 )     entry_height = DEFAULT_ENTRYHEIGHT;	

	if( border_color == NULL ) {
		char* tmp = (char*)malloc( sizeof( DEFAULT_BORDERCOLOR ) );
		strncpy( tmp, DEFAULT_BORDERCOLOR, strlen( DEFAULT_BORDERCOLOR ) );
		border_color = tmp;
	}

	if( bgColor == NULL ) {
		char* tmp = (char*)malloc( sizeof( DEFAULT_BORDERCOLOR ) );
		strncpy( tmp, DEFAULT_BACKGROUND, strlen( DEFAULT_BACKGROUND ) );
		bgColor = tmp;
	}
	if( fgColor == NULL ) {
		char* tmp = (char*)malloc( sizeof( DEFAULT_BORDERCOLOR ) );
		strncpy( tmp, DEFAULT_FOREGROUND, strlen( DEFAULT_FOREGROUND ) );
		fgColor = tmp;
	}
	if( fontname == NULL ) {
		char* tmp = (char*)malloc( sizeof( DEFAULT_FONTNAME ) );
		strncpy( tmp, DEFAULT_FONTNAME, strlen( DEFAULT_FONTNAME ) );
		fontname = tmp;
	}
	if( focus_bgColor == NULL ) {
		char* tmp = (char*)malloc( sizeof( DEFAULT_FOCUSBACKGROUND ) );
		strncpy( tmp, DEFAULT_FOCUSBACKGROUND, strlen( DEFAULT_FOCUSBACKGROUND ) );
		focus_bgColor = tmp;
	}
	if( focus_fgColor == NULL ) {
		char* tmp = (char*)malloc( sizeof( DEFAULT_FOCUSFOREGROUND ) );
		strncpy( tmp, DEFAULT_FOCUSFOREGROUND, strlen( DEFAULT_FOCUSFOREGROUND ) );
		focus_fgColor = tmp;
	}
}

/* getLevel: "tab character" decide level of menu - please look a configuration example file.    */
extern int 
getLevel(char* str)
{
	int rValue = 0;

	while( !isalpha(*str) ) 
        {
		if( *str == '\t' ) rValue++;
		str++;
	}

	return rValue;
}

/* numberOfMenu: return the number of menu, which contains sub entries. */
extern int
numberOfMenu(int level)
{
	ENTRY*  cursor;
	int     total = 0,                      \
                index;

	cursor   = menu;
	
        for( index = 0; index < level; index++ ) 
        {
		cursor = menu->child;
	}
	
	if( cursor == NULL ) 
                return 0;

	while( cursor != NULL ) 
        {
		cursor = cursor->next;
		total++;
	}
	
        return total;
}

extern ENTRY* 
makeNewEntry(char* str)
{
	char*    ptr;
	ENTRY*   newentry;

	/* firstly, initialize entry title as str */		
	newentry = (ENTRY*)malloc(sizeof(ENTRY));
	ptr      = strtok( str, "," );
	
        newentry->name    = trimTabspace(ptr);	/* name */
	newentry->command = NULL;
	newentry->child   = NULL;
	newentry->parent  = NULL;
	newentry->xpmPath = NULL;
	newentry->prev    = NULL;
	newentry->next    = NULL;
	newentry->level   = getLevel(ptr);

	if( (ptr = strtok( NULL, "," )) != NULL ) {
		/* if str has a command */
		newentry->command = trimTabspace(ptr);
		
		/* set child pointers(ptr for sub directory) as null */
		newentry->child   = NULL;
	}

	if( menu == NULL ) {
                /* first entry location will be saved in menu ptr(global value) */
		menu      = newentry;
		lastentry = newentry;
	}
	else {
		if( newentry->level == lastentry->level ) { /* new entry in same level */
			lastentry->next  = newentry;
			newentry->parent = lastentry->parent; 
			newentry->prev   = lastentry; 
		}
		else if( newentry->level > lastentry->level ) { /* new entry in sub level */
			lastentry->child = newentry; 
			newentry->parent = lastentry; 
		}
		else if( newentry->level < lastentry->level ) { /* new entry in parent level */
                        {
                                /* move to upper level */
                                int i;
                                int max = lastentry->level - newentry->level;
                                for(i = 0; i < max; i++ ) 
                                        lastentry = lastentry->parent; 
                        }
                        newentry->parent = lastentry->parent;
			lastentry->next = newentry; 
		}

	}
        lastentry = newentry;
                
	return newentry;
}



/* Read configuration file and init menu entries. */
extern void 
initConfig(FILE* fp)
{
	char   buf[BUFSIZE];
	char*  pch;
	bool   init_menu = false; /* flag for representing to read entry nodes( name, command, xpm path ) */
	
	/* initialize global value as minus, NULL */
	menu_width   = -1;
	menu_height  = -1;
	menu_padding = -1;
	border_width = -1;
	entry_height = -1;
	border_color = NULL;
	bgColor      = NULL;
	fgColor      = NULL;
	fontname     = NULL;
	focus_bgColor= NULL;
	focus_fgColor= NULL;
	
	/* setting global variation from configuration file */
	while( fgets(buf, BUFSIZE, fp) > 0 ) {
                /* start to tokenize the string */
                pch = strtok( buf, TOKEN_VALUES );

		while ( pch != NULL ) {
			/* make string from configuration file be lower */
			tolowerString(pch);


			if( ISOPTION(OPTION_MENUWIDTH) ) {
				pch = strtok( NULL, TOKEN_VALUES );
				COPY_INTVALUE(menu_width)
			}
			else if( ISOPTION(OPTION_MENUHEIGHT)) {
				pch = strtok( NULL, TOKEN_VALUES );
				COPY_INTVALUE(menu_height)
			}
			else if( ISOPTION(OPTION_MENUPADDING)) {
				pch = strtok( NULL, TOKEN_VALUES );
				COPY_INTVALUE(menu_padding)
			}
			else if( ISOPTION(OPTION_BORDERWIDTH)) {
				pch = strtok( NULL, TOKEN_VALUES );
				COPY_INTVALUE(border_width)
			}
			else if( ISOPTION(OPTION_BORDERCOLOR) ){
				pch = strtok( NULL, TOKEN_VALUES );
				COPY_STRVALUE(border_color)
			}
			else if( ISOPTION(OPTION_ENTRYHEIGHT) ) {
				pch = strtok( NULL, TOKEN_VALUES );
				COPY_INTVALUE(entry_height);
			}
			else if( ISOPTION(OPTION_BACKGROUND) ) {
				pch = strtok( NULL, TOKEN_VALUES );
				COPY_STRVALUE(bgColor)
			}
			else if( ISOPTION(OPTION_FOREGROUND)) {
				pch = strtok( NULL, TOKEN_VALUES );
				COPY_STRVALUE(fgColor)
			}
			else if( ISOPTION(OPTION_FONTNAME)) {
				pch = strtok( NULL, TOKEN_FONTNAME );
				COPY_STRVALUE(fontname);
			}
			else if( ISOPTION(OPTION_FOCUSBACKGROUND)) {
				pch = strtok( NULL, TOKEN_VALUES );
				COPY_STRVALUE(focus_bgColor);
			}
			else if( ISOPTION(OPTION_FOCUSFOREGROUND)) {
				pch = strtok( NULL, TOKEN_VALUES );
				COPY_STRVALUE(focus_fgColor);
			}
			else if( ISOPTION(OPTION_TEXTALIGN) ) {
				pch = strtok( NULL, TOKEN_VALUES );
                                
				if(      strncmp(pch, "left", 4) == 0   ) align = LEFT;
				else if( strncmp(pch, "right", 4) == 0  ) align = RIGHT;
				else if( strncmp(pch, "center", 4) == 0 ) align = CENTER;
				else                                      align = DEFAULT_ALIGN; /* DEFAULT_ALIGN = LEFT */
			}
			else if( !init_menu && ISOPTION(OPTION_MENU) ) {
				/* start parsing all entries in configuration file */
				fgets(buf, BUFSIZE, fp); /* drop the "menu" */
				tolowerString(buf);

				while ( !ISENDMENU(buf) ){
					/********************** make entry nodes ***********************/
#ifdef _DEBUG_ON_
					ENTRY* newentry = makeNewEntry(buf);
					showEntry(newentry);
#else
                                        makeNewEntry(buf);
#endif
					/***************************************************************/
					fgets(buf, BUFSIZE, fp);
					tolowerString(buf);
				}
			}

			else {
				pch = strtok( NULL, TOKEN_VALUES );
			}
		}

	}
}


extern char* 
trimTabspace(char* str)
{
	int index, count=0;
	char* copied;
	
	for( index = 0; index < strlen(str); index++ ) {
		if( !isspace(str[index]) ) break;
		else if( str[index] == '\t' || str[index] == '\n' || str[index] == '\'' || str[index] == '\"' ) count++;
	}
	
	copied = (char*)malloc(sizeof(char)*(strlen(str) - count + 1));
	for( index = 0, count = 0; index < strlen(str); index++ ) {
		if( str[index] != '\t' && str[index] != '\n' && str[index] != '\'' &&  str[index] != '\"' ) {
			copied[count] = str[index];
			count++;
		}
	}
	copied[count] = '\0';
	
	return copied;
}
extern void 
tolowerString(char* str)
{
	int index = 0;

	for( ; index < strlen(str); index++ ) {
		str[index] = tolower(str[index]);
	}
}
int main(void)
{	
	initmenu();
#ifdef _DEBUG_ON_
        showValues();
#endif
	checkValues();
	return draw();
}


#ifdef _DEBUG_ON_

void
showMenu(ENTRY* ent)
{
	ENTRY* cursor = ent;
	if( cursor == NULL ) return;
	else {
		showEntry(cursor);
		if( cursor->child != NULL ) showMenu(cursor->child);
		if( cursor->next != NULL ) showMenu(cursor->next);
	}

}
void 
showValues(void)
{
	printf("=======================================================\n");
	printf("menu_width\t%d\n", menu_width);
	printf("menu_height\t%d\n", menu_height);
	printf("border_width\t%d\n", border_width);
	printf("border_color\t%s\n", border_color);
	printf("entry_height\t%d\n", entry_height);
	printf("background\t%s\n", bgColor);
	printf("foreground\t%s\n", fgColor);
	printf("text_align\t%d\n", align);
	printf("fontname\t%s\n", fontname);
	printf("=======================================================\n");
}
void 
showEntry(ENTRY* target)
{
	fprintf(stdout, "name : %s\n", target->name);
	fprintf(stdout, "command : %s\n", target->command);
	fprintf(stdout, "child : %p\n", target->child);
	fprintf(stdout, "parent : %p\n", target->parent);
	fprintf(stdout, "prev : %p\n", target->prev);
	fprintf(stdout, "next : %p\n", target->next);
	fprintf(stdout, "level : %d\n", target->level);
}
#endif
