/***
*os2/stat.c - OS/2 get file status
*
*   Copyright (c) 1985-1988, Microsoft Corporation.  All rights reserved.
*
*Purpose:
*   defines stat() - get file status
*
*******************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>
#include <msdos.h>
#include <dostypes.h>
#include <register.h>
#include <dos.h>
#include <doscalls.h>
#include <string.h>
#include <internal.h>
#include <stdlib.h>
#include <direct.h>


#define STRPBRK strpbrk

#define ISSLASH(a)  ((a) == '\\' || (a) == '/')

#define EXT(a) stricmp(p,a)

/***
*static unsigned near _dtoxmode(attr, name) -
*
*Purpose:
*
*Entry:
*
*Exit:
*
*Exceptions:
*
*******************************************************************************/

static unsigned near _dtoxmode(int attr, char *name)
{
    REG1 unsigned uxmode;
    unsigned dosmode;
    REG2 char *p;

    dosmode = attr & 0xff;
    if ((p = name)[1] == ':')
        p += 2;

    /* check to see if this is a directory - note we must make a special
     * check for the root, which DOS thinks is not a directory
     */

    uxmode = (( ISSLASH(*p) && !p[1]) || (dosmode & A_D)
        || !*p) ? S_IFDIR|S_IEXEC : S_IFREG;

    /* If attribute byte has system bit set, assume read-only */

    uxmode |= (dosmode & (A_S|A_RO)) ? S_IREAD : (S_IREAD|S_IWRITE);

    /* see if file appears to be executable - check extension of name */

    if (p = strrchr(name, '.')) {
        if ( !EXT(".exe") || !EXT(".bat") || !EXT(".com") )
            uxmode |= S_IEXEC;
    }

    /* propagate user read/write/execute bits to group/other fields */

    uxmode |= (uxmode & 0700) >> 3;
    uxmode |= (uxmode & 0700) >> 6;

    return(uxmode);
}


/***
*int stat(name, buf) - get file status info
*
*Purpose:
*   stat obtains information about the file and stores it in the structure
*   pointed to by buf.
*
*Entry:
*   char *name -    pathname of given file
*   struct stat *buffer - pointer to buffer to store info in
*
*Exit:
*   fills in structure pointed to by buffer
*   returns 0 if successful
*   returns -1 and sets errno if unsuccessful
*
*Exceptions:
*
*******************************************************************************/


int  stat( REG1 char *name, REG2 struct stat *buf)
{
    struct FileFindBuf findbuf;
    unsigned findcount = 1;     /* Find only 1 match */
    unsigned findhandle = -1;   /* any unused handle */

    char * oldcwd;          /* Pointer to current directory */

    unsigned long dmap;     /* Valid Drive Map (ignored) */
    int drive;          /* A: = 1, B: = 2, etc. */

    /* Don't allow wildcards to be interpreted by system */

    if (strpbrk(name, "?*")) {
        errno = ENOENT;
        _doserrno = E_nofile;
        return(-1);
    }

    /* Try to get disk from name.  If none, get current disk.  */

    if (name[1] == ':'){

        drive = tolower(*name) - 'a' + 1;
    }
    else
        (void) DOSQCURDISK((unsigned far *) &drive,
        (unsigned long far *) &dmap);

    /* Call Find Match File */

    errno = ENOENT;

    if (DOSFINDFIRST((char far *)name, (unsigned far *)&findhandle, A_D|A_S|A_H,
    (struct FileFindBuf far *) &findbuf, sizeof(findbuf),
    (unsigned far *) &findcount, 0L)) {

        if ( STRPBRK(name, "./\\") ) {  /* Possible root directory? */
            if ( ! ( oldcwd = _getdcwd(drive, NULL, -1) ) )
                return( -1 );
            if ( chdir( name ) != -1 ) {
                chdir( oldcwd ); /* Must be a root directory */
                free( oldcwd );
            }
            else {
                free( oldcwd );
                return( -1 );
            }

            findbuf.attributes = A_D;
            findbuf.file_size = 0L;
            findbuf.write_date = (1 << 5) + 1; /* 1 January 1980 */
            findbuf.write_time = 0;        /* 00:00:00 */
            findbuf.access_date =
                findbuf.create_date =
                findbuf.access_time =
                findbuf.create_time = 0;
        }
        else
            return( -1 );

    }
    else
        DOSFINDCLOSE(findhandle);   /* Release Find handle */

    /* Fill in buf */

    buf->st_uid = buf->st_gid = buf->st_ino = 0;

    buf->st_rdev = buf->st_dev = drive - 1; /* A=0, B=1, etc. */

    /* now set the common fields */

    buf->st_mode = _dtoxmode(findbuf.attributes, name);
    buf->st_nlink = 1;
    buf->st_size = findbuf.file_size;
    buf->st_mtime = XTIME(findbuf.write_date, findbuf.write_time);

    /*
     * If create and access times are 0L, use modification time instead
     */
    if( findbuf.create_date || findbuf.create_time )
        buf->st_ctime = XTIME(findbuf.create_date, findbuf.create_time);
    else
        buf->st_ctime = buf->st_mtime;

    if( findbuf.access_date || findbuf.access_time )
        buf->st_atime = XTIME(findbuf.access_date, findbuf.access_time);
    else
        buf->st_atime = buf->st_mtime;

    return(0);
}
