/* Commodore LCD emulator using SDL2 library. Also includes:
   Test-case for a very simple and inaccurate Commodore VIC-20 emulator.
   Copyright (C)2016 LGB (Gábor Lénárt) <lgblgblgb@gmail.com>

   The goal of emutools.c is to provide a relative simple solution
   for relative simple emulators using SDL2.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#ifndef __LGB_EMUTOOLS_H_INCLUDED
#define __LGB_EMUTOOLS_H_INCLUDED

#include <SDL.h>

#ifdef __GNUC__
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#else
#define likely(x)       (x)
#define unlikely(x)     (x)
#endif

#define APP_ORG "xemu-lgb"
#define APP_DESC_APPEND " / LGB"

// You should define this in your emulator, most probably with resetting the keyboard matrix
// Purpose: emulator windows my cause the emulator does not get the key event normally, thus some keys "seems to be stucked"
extern void clear_emu_events ( void );

extern void emu_drop_events ( void );


/* Note: O_BINARY is a must for Windows for opening binary files, odd enough, I know ...
         So we always use O_BINARY in the code, and defining O_BINARY as zero for non-Windows systems, so it won't hurt at all.
	 Surely, SDL has some kind of file abstraction layer, but I seem to get used to some "native" code as well :-) */
#ifndef _WIN32
#	define O_BINARY		0
#	define DIRSEP_STR	"/"
#	define DIRSEP_CHR	'/'
#	define NL		"\n"
#else
#	define DIRSEP_STR	"\\"
#	define DIRSEP_CHR	'\\'
#	define NL		"\r\n"
#endif

#define _REPORT_WINDOW_(sdlflag, str, ...) do { \
	char _buf_for_win_msg_[4096]; \
	snprintf(_buf_for_win_msg_, sizeof _buf_for_win_msg_, __VA_ARGS__); \
	fprintf(stderr, str ": %s" NL, _buf_for_win_msg_); \
	SDL_ShowSimpleMessageBox(sdlflag, sdl_window_title, _buf_for_win_msg_, sdl_win); \
	clear_emu_events(); \
	emu_drop_events(); \
	emu_timekeeping_start(); \
} while (0)

#define INFO_WINDOW(...)	_REPORT_WINDOW_(SDL_MESSAGEBOX_INFORMATION, "INFO", __VA_ARGS__)
#define WARNING_WINDOW(...)	_REPORT_WINDOW_(SDL_MESSAGEBOX_WARNING, "WARNING", __VA_ARGS__)
#define ERROR_WINDOW(...)	_REPORT_WINDOW_(SDL_MESSAGEBOX_ERROR, "ERROR", __VA_ARGS__)

#define FATAL(...) do { \
	ERROR_WINDOW(__VA_ARGS__); \
	exit(1); \
} while (0)

extern FILE *debug_fp;

#define DEBUG(...) do { \
	if (unlikely(debug_fp))	\
		fprintf(debug_fp, __VA_ARGS__);	\
} while (0)


#ifndef __BIGGEST_ALIGNMENT__
#define __BIGGEST_ALIGNMENT__	16
#endif
#define VARALIGN __attribute__ ((aligned (__BIGGEST_ALIGNMENT__)))

extern char *sdl_window_title;
extern char *window_title_custom_addon;
extern SDL_Window   *sdl_win;
extern Uint32 sdl_winid;
extern SDL_PixelFormat *sdl_pix_fmt;
extern int seconds_timer_trigger;
extern const char emulators_disclaimer[];

extern int emu_init_debug ( const char *fn );
extern time_t emu_get_unixtime ( void );
extern struct tm *emu_get_localtime ( void );
extern void *emu_malloc ( size_t size );
extern char *emu_strdup ( const char *s );
extern int emu_load_file ( const char *fn, void *buffer, int maxsize );
extern void emu_set_full_screen ( int setting );
extern void emu_timekeeping_delay ( int td_em );
extern int emu_init_sdl (
        const char *window_title,               // title of our window
        const char *app_organization,           // organization produced the application, used with SDL_GetPrefPath()
        const char *app_name,                   // name of the application, used with SDL_GetPrefPath()
        int is_resizable,                       // allow window resize? [0 = no]
        int texture_x_size, int texture_y_size, // raw size of texture (in pixels)
        int logical_x_size, int logical_y_size, // "logical" size in pixels, ie to correct aspect ratio, etc, can be the as texture of course, if it's OK ...
        int win_x_size, int win_y_size,         // default window size, in pixels [note: if logical/texture size combo does not match in ratio with this, black stripes you will see ...]
        Uint32 pixel_format,                    // SDL pixel format we want to use (an SDL constant, like SDL_PIXELFORMAT_ARGB8888) Note: it can gave serve impact on performance, ARGB8888 recommended
        int n_colours,                          // number of colours emulator wants to use
        const Uint8 *colours,                   // RGB components of each colours, we need 3 * n_colours bytes to be passed!
        Uint32 *store_palette,                  // this will be filled with generated palette, n_colours Uint32 values will be placed
        int render_scale_quality,               // render scale quality, must be 0, 1 or 2 _ONLY_
        int locked_texture_update,              // use locked texture method [non zero], or malloc'ed stuff [zero]. NOTE: locked access doesn't allow to _READ_ pixels and you must fill ALL pixels!
        void (*shutdown_callback)(void)         // callback function called on exit (can be nULL to not have any emulator specific stuff)
);
extern void emu_timekeeping_start ( void );
extern void emu_render_dummy_frame ( Uint32 colour, int texture_x_size, int texture_y_size );
extern Uint32 *emu_start_pixel_buffer_access ( int *texture_tail );
extern void emu_update_screen ( void );

#endif
