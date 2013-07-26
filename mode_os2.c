/* access extended mode bits */

#define INCL_NOPM
#include <os2.h>


int getfmode(char *name)
{
  USHORT mode;
  return DosQFileMode(name, &mode, 0L) ? -1 : mode;
}


int setfmode(char *name, unsigned mode)
{
  return DosSetFileMode(name, mode, 0L) ? -1 : 0;
}
