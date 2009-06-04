/***********************************************************************
* Copyright (C) 2002,2003,2004,2005,2006,2007,2008 Carsten Urbach
*
* This file is part of tmLQCD.
*
* tmLQCD is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* tmLQCD is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with tmLQCD.  If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/

#include "utils.ih"

int read_checksum_parallel(LemonReader * lemonreader, DML_Checksum * checksum)
{
  char *message = 0, *pos = 0;
  uint64_t bytes, bytes_read;

  bytes = lemonReaderBytes(lemonreader);
  bytes_read = bytes;
  message = (char*)malloc(bytes + 1); /* Allow for an explicit closing byte. */

  lemonReaderReadData(message, &bytes_read, lemonreader);
  message[bytes] = '\0';

  /* So, yeah, that means we're going to have to parse the XML */
  pos = strtok(message, "<> \n\t");

  while (pos)
  {
    if (!strncmp(pos, "suma", 4))
    {
      pos = strtok(0, "<> \n\t");
      sscanf(pos, "%x", &checksum->suma);
    }
    if (!strncmp(pos, "sumb", 4))
    {
      pos = strtok(0, "<> \n\t");
      sscanf(pos, "%x", &checksum->sumb);
    }
    pos = strtok(0, "<> \n\t");
  }

  free(message);
  return(0);
}