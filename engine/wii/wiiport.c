/*
 * OpenBOR - http://www.LavaLit.com
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in OpenBOR root for details.
 *
 * Copyright (c) 2004 - 2011 OpenBOR Team
 */

#include <ogcsys.h>
#include <stdlib.h>
#include <string.h>
#include "wiiport.h"
#include "packfile.h"
#include "video.h"
#include "control.h"
#include "utils.h"
#include "ram.h"
#include "menu.h"

#if USE_LIBELM
#include <elm.h>
#else
#include <fat.h>
#endif

#if USE_USYNC // mutex/syncronization library used by libELM; licensed under the LGPL
#include <usync.h>
#endif

char packfile[MAX_FILENAME_LEN] = {"sd:/apps/OpenBOR/bor.pak"};
char paksDir[MAX_FILENAME_LEN];
char savesDir[MAX_FILENAME_LEN];
char logsDir[MAX_FILENAME_LEN];
char screenShotsDir[MAX_FILENAME_LEN];
char rootDir[MAX_FILENAME_LEN]; // note: this one ends with a slash

/*
 * Given a file's path relative to the OpenBOR executable; returns an absolute
 * path that can be opened with fopen. For example, Logs/OpenBorLog.txt could become
 * sd:/apps/OpenBOR/Logs/OpenBorLog.txt or usb:/apps/OpenBOR/Logs/OpenBorLog.txt.
 */
char* getFullPath(char *relPath)
{
	static char filename[MAX_FILENAME_LEN];
	strcpy(filename, rootDir);
	strcat(filename, relPath);
	return filename;
}

void borExit(int reset)
{
#if 0
	if(reset == WII_SHUTDOWN)   SYS_ResetSystem(SYS_POWEROFF, 0, 0);
	else if(reset == WII_RESET) SYS_ResetSystem(SYS_HOTRESET, 0, 0);
	else exit(reset);
#else
    exit(reset); //SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
#endif
}

// TODO merge into getFullPath
void initDirPath(char* dest, char* relPath)
{
	strcpy(dest, rootDir);
	strcat(dest, relPath);
}

int main(int argc, char *argv[])
{
	video_init();

#if USE_USYNC
	uSyncInit(); // must be called before ELM_Mount() if libELM is compiled with µSync support
#endif

#if USE_LIBELM // use libELM for FAT filesystem access
	ELM_Mount();
#else // use libfat for FAT filesystem access
	fatInitDefault();
#endif

	setSystemRam();
	packfile_mode(0);

#if 0
	strncpy(rootDir, argv[0], strrchr(argv[0], '/') - argv[0]);
	initDirPath(paksDir, "Paks");
	initDirPath(savesDir, "Saves");
	initDirPath(logsDir, "Logs");
	initDirPath(screenShotsDir, "ScreenShots");
#else
	if(strstr(argv[0], "usb:/") != NULL)
	{
		strcpy(rootDir, "usb:/apps/OpenBOR/");
		strcpy(paksDir, "usb:/apps/OpenBOR/Paks");
		strcpy(savesDir, "usb:/apps/OpenBOR/Saves");
		strcpy(logsDir, "usb:/apps/OpenBOR/Logs");
		strcpy(screenShotsDir, "usb:/apps/OpenBOR/ScreenShots");
	}
	else
	{
		strcpy(rootDir, "sd:/apps/OpenBOR/");
		strcpy(paksDir, "sd:/apps/OpenBOR/Paks");
		strcpy(savesDir, "sd:/apps/OpenBOR/Saves");
		strcpy(logsDir, "sd:/apps/OpenBOR/Logs");
		strcpy(screenShotsDir, "sd:/apps/OpenBOR/ScreenShots");
	}
#endif

	dirExists(paksDir, 1);
	dirExists(savesDir, 1);
	dirExists(logsDir, 1);
	dirExists(screenShotsDir, 1);

	Menu();
	openborMain(argc, argv);
	borExit(0);
	return 0;
}

