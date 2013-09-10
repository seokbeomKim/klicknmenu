#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "types.h"
#include "draw.h"
#include "klicknmenu.h"

/* Create window */
void OpenWindow(DpyContent* display, Window parent)
{
        XSetWindowAttributes theWindowAttributes;
        unsigned long        theWindowMask;
        XSizeHints           theSizeHints;
        Window               theNewWindow;

        /*Setting the attributes*/
        theWindowAttributes.border_pixel
                      = getColor(display, border_color);
        theWindowAttributes.background_pixel
          	      = getColor(display, bgColor);
        theWindowAttributes.override_redirect = True;

        theWindowMask = CWBackPixel|CWBorderPixel|CWOverrideRedirect;

        /* create window */
        theNewWindow = XCreateWindow( display->dpy,
                                      parent,
                                      display->x,display->y,display->width,display->height,
                                      border_width,display->depth,
                                      InputOutput,
                                      CopyFromParent,
                                      theWindowMask,
                                      &theWindowAttributes);

        theSizeHints.flags  = PPosition | PSize;
        theSizeHints.x      = display->x;
        theSizeHints.y      = display->y;
        theSizeHints.width  = display->width;
        theSizeHints.height = display->height;

        XSetNormalHints(display->dpy,theNewWindow,&theSizeHints);
	display->win = theNewWindow;
	XSelectInput(display->dpy, display->win, 
		     Button2MotionMask|
		     EnterWindowMask|LeaveWindowMask|
		     PointerMotionMask|ButtonPressMask);
        XMapWindow(display->dpy,theNewWindow);
}

extern int
openDisplay(DpyContent* display)
{
	if ( (display->dpy = XOpenDisplay(NULL) ) == NULL ) {
		return 1;
	}
	else {
		display->screen = DefaultScreen(display->dpy);
		display->depth  = DefaultDepth (display->dpy, display->screen);

		return 0;
	}
}

extern int
closeDisplay(DpyContent* display)
{
	return XCloseDisplay(display->dpy);
}

/* Get the pixel variable */
extern unsigned long
getColor(DpyContent* display, char* colorstr)
{
	XColor color;
	Colormap cmap = DefaultColormap(display->dpy, display->screen);

	XAllocNamedColor(display->dpy, cmap, colorstr, &color, &color);
	return color.pixel;
}

/* return number of entires in the same level */
extern int
getEntryCount(ENTRY* entry)
{
	ENTRY* cursor = entry;
	int    num    = 0;

	
	while( cursor != NULL ) {
		cursor = cursor->next;
		num++;
	}

	return num;
}

extern void
initdraw(DpyContent* display)
{
	mouse_pos            mice_ptr;
	
	if ( openDisplay( display ) ) {
		perror("Cannot open the display.");
		exit(5);
	}

	/* set the miscelleneous values from those values which were read from config file. */
	/* adjust the proper size */
	
	menu_height = entry_height * numberOfMenu(0);

	display->width  = menu_width;
	display->height = menu_height;

	getMice(&mice_ptr,display); /* get the mouse pointer position */

	display->x = mice_ptr.x;
	display->y = mice_ptr.y;
	
	display->next = NULL;
	display->prev = NULL;
}

/* return value for x position which is value for printing text */
extern int
alignPosition(DpyContent* display, char* str)
{
        int rValue;
	if( align == LEFT ) rValue = menu_padding;
	else if( align == CENTER ) {
		rValue =  display->width / 2 
			- XTextWidth(display->fontinfo, str, strlen(str)) / 2;
	}
	else if( align == RIGHT ) 
		rValue = menu_width - menu_padding - 
			XTextWidth(display->fontinfo, str, strlen(str));
        
        return rValue;
}

/* Make the GC and draw entries */
extern void
drawEntry(ENTRY* entry, DpyContent* display, ENTRY* current)
{
	int           count = 0;
	XGCValues     values;
	ENTRY*        cursor = entry;

	/* set font and foreground color */
	values.background = getColor(display, bgColor);
	values.foreground = getColor(display, fgColor);


	/* get the font information by querying font name */
	display->fontinfo = XLoadQueryFont(display->dpy, fontname);
	display->gc   = XCreateGC( display->dpy, display->win, GCForeground | GCBackground, &values );
        XSetFont (display->dpy, display->gc, display->fontinfo->fid);

        if( !display->fontinfo ) {
                fprintf(stderr,"unable to use %s\n", fontname);
        }

	while ( cursor != NULL ) {
                if ( cursor == current ) {
                        XSetForeground(display->dpy, display->gc, getColor( display, focus_fgColor ) );
                        XSetBackground(display->dpy, display->gc, getColor( display, focus_bgColor ) );
                }
                else {
                        XSetForeground(display->dpy, display->gc, getColor( display, fgColor ) );                        
                        XSetBackground(display->dpy, display->gc, getColor( display, bgColor ) );
                }
		XDrawString( display->dpy, display->win, display->gc, 
			     alignPosition(display, cursor->name),    /* x position */
			     menu_padding + entry_height * count + ( entry_height / 2 ), /* y position */
			     cursor->name, strlen(cursor->name));
		cursor = cursor->next;
		count++;
	}
}

/* Make the new DpyContent type with position values. */
extern DpyContent*
newDpyContent(DpyContent* dpy, int x, int y, int width, int height)
{
	DpyContent* temp;
	temp = (DpyContent*) malloc(sizeof(DpyContent));
	temp->x = x;
	temp->y = y;
	temp->width = width;
	temp->height = height;
	temp->dpy = dpy->dpy;
	temp->screen = dpy->screen;
	temp->depth = dpy->depth;
	temp->prev = dpy;
	temp->next = NULL;
	dpy->next = temp;
	
	return temp;
}

extern int 
draw(void)
{
	DpyContent   dc;
	XEvent       ev;
	DpyContent*  newDpy;
	int          height_in_window; /* the mouse position(y) in the window */

        /* values for condition */
	bool         entered = false;
	int          entry_num = -1;
	int          previous_level; /* previous level: previous pointed menu level */

        /* values for tracking nodes */
	int          index;
	int          counter = 0;
	DpyContent*  dpycursor;
	ENTRY*       cursor;
	
	initdraw(&dc);          /* make the head DpyContent */

	OpenWindow(&dc, RootWindow( dc.dpy, dc.screen ));
	dc.first_entry = menu;  /* DpyContent head <- dc */
	drawEntry(menu, &dc, NULL);



	XAllowEvents(dc.dpy, AsyncBoth, CurrentTime);
	previous_level = 0;

	while( true )
        {
		XNextEvent(dc.dpy, &ev);

		switch(ev.type){
		case MotionNotify:
			if( entered ) {
				dpycursor = &dc;
				counter = 0;
				while ( ev.xclient.window != dpycursor->win ) {
					dpycursor = dpycursor->next;
					counter++;
				}
#ifdef _DEBUG_ON_
				printf("previous level: %d Found level : %d\n", previous_level, counter);
				printf("Found the proper window and first entry is : %s\n", dpycursor->first_entry->name);
#endif
				/* get the mouse pointer position */
				XGrabPointer(dpycursor->dpy, 
					     dpycursor->win,
					     1, 
					     PointerMotionMask | ButtonPressMask | ButtonReleaseMask , 
					     GrabModeAsync,
					     GrabModeAsync, 
					     None,
					     None,
					     CurrentTime);
				height_in_window = ev.xmotion.y_root - dpycursor->y;

				/* find the proper entry from the entry list in the dpycursor */
				cursor = dpycursor->first_entry;

				for( index = 0; index < height_in_window / entry_height; index++ ) {
					cursor = cursor->next;
				}

#ifdef _DEBUG_ON_
				fprintf(stdout,"partition num : %d\n", height_in_window / entry_height);
#endif
				
                                if( height_in_window / entry_height == getEntryCount(dpycursor->first_entry) ){
                                        break;
                                }
				/* if currently focusing entry is different with before */
				if(entry_num != (height_in_window / entry_height ) || previous_level != counter ) {
					if( dpycursor->next != NULL ) {
						/* recursively destroywindow */
						int min = previous_level < counter? previous_level:counter;
						DpyContent* target = &dc;
						int index;
						for( index = 0; index < min; index++ ) {
							target = target->next;
						}
						
						destroyWindow( dpycursor->next );
					}

                                        drawEntry( dpycursor->first_entry, dpycursor, cursor );
					if( cursor->child != NULL ) {
						newDpy = newDpyContent(dpycursor,
								       dpycursor->x + menu_width + border_width,
								       dpycursor->y + border_width + entry_height * (height_in_window / entry_height),
								       menu_width,
								       entry_height * getEntryCount(cursor->child));
						newDpy->first_entry = cursor->child;
						newDpy->prev = dpycursor;
						drawSubmenu(cursor->child, newDpy, RootWindow( newDpy->dpy, newDpy->screen ));


						entry_num = height_in_window / entry_height;
						previous_level = counter;
					}
					else {
						/* there is no more child */
						entry_num = height_in_window / entry_height;
						previous_level = counter;
					}
				}

			}
			break;
		case EnterNotify:
			entered = true;
			break;
		case LeaveNotify:
			entered = false;
			break;
		case ButtonRelease:
			if( cursor->command != NULL ) {
				freedraw(&dc);
				closeDisplay(&dc);

				system(cursor->command);
				return 0;
			}
			break;
		case ButtonPress:
			if( !entered || ev.xbutton.button == 3 ) {
				freedraw(&dc);
				closeDisplay(&dc);
				return 0;
			}
		}
	}
}

extern void
destroyWindow( DpyContent* display ) {
	if( display->next != NULL ) {
		destroyWindow( display->next );
	}
	if( display != NULL ) {
		XFreeGC( display->dpy, display->gc);
		XDestroyWindow( display->dpy, display->win );
		display->prev->next = NULL;
		free( display );
	}
}

extern void
drawSubmenu(ENTRY* parent, DpyContent* display, Window window)
{
	OpenWindow(display, window);
	drawEntry( parent, display, NULL );
}

extern void
freedraw(DpyContent* display)
{
	XFreeGC(display->dpy, display->gc);
	XDestroyWindow(display->dpy, display->win);
}

extern void
getMice(mouse_pos* pos, DpyContent* display)
{
	Window window_returned;
	int root_x, root_y, win_x, win_y;
	unsigned int mask_return;
	
	XQueryPointer(display->dpy, XDefaultRootWindow(display->dpy), &window_returned,
                &window_returned, &root_x, &root_y, &win_x, &win_y,
                &mask_return);

	pos->x = root_x;
	pos->y = root_y;
}


