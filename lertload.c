/*
   file: lertload.c
   basic use: lertload a < file_of_names
     puts a set of names with a single letter category in the lert_msg db

 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <ctype.h>

#include "lert.h"     

int main(int argc, char **argv)
{
  char buffer[512];
  DBM *db;
  datum key;
  datum old;
  datum data;
  char *cp;
  char *bp;
  char *nd_p;
  int nd_c;
  int count;
  int new_char;
  char categ[128];

  if (argc != 2) {
     fprintf(stderr, "usage: %s type\n", argv[0]);
     fprintf(stderr, "   type is a single character category\n");
     fprintf(stderr, "   and a file of names is fed in as stdin\n");
     fprintf(stderr, "   and a matching lertdata file is written\n");
     exit(1);
  }
    
  db = dbm_open(LERTS_DATA, O_RDWR|O_CREAT, 0600);
  if (db == NULL) {
    fprintf(stderr, "Unable to open database file %s.\n", LERTS_DATA);
    exit (1);
  }
  while(fgets(buffer, 512, stdin) != NULL) {
    bp = buffer;
    while (isspace(*bp))
      bp++; /* skip leading spaces */
    cp = bp;
    while (!isspace(*cp) && *cp != '\0')
      cp++;
    *cp = '\0';
    key.dptr = bp;
    key.dsize = strlen(bp) + 1;


    old = dbm_fetch(db, key);

    if (old.dptr == NULL) {
      categ[0] = argv[1][0];
      nd_p = categ;
      data.dptr = categ;
      nd_c = 1;
    } else {
      cp = old.dptr;
      count = old.dsize;
      data.dptr = (char *)malloc(old.dsize + 1);

      nd_p = data.dptr;
      nd_c = 0;
      new_char = TRUE;

      while (count > nd_c) {
        *nd_p = *cp;
/*
   already in list?
 */
        if (*cp == argv[1][0]) {
          new_char = FALSE;
	}
      nd_p++;
      cp++;
      nd_c++;
      }

      if (new_char) {
        *nd_p = argv[1][0];
        nd_c++;
      }
    }
/*
    data.dptr = nd_p;
 */
    data.dsize = nd_c;
    if (dbm_store(db, key, data, DBM_REPLACE) < 0) {
      fprintf(stderr, "dbm_store() failed: Entry = %d\n", key.dptr);
      (void) dbm_clearerr(db);
    }
  }
  return (0);
}
    
