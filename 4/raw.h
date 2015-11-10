#ifndef RAW_H
#define RAW_H

/* These functions switch the terminal between the "raw" and "cooked"
 * modes.  By default, a terminal is in "cooked".  These functions
 * therefore assume that you will call raw_mode() first.  You should
 * call cooked_mode() just before your program exits.  You can set
 * this up using the atexit(3) function, or just call it manually.
 *
 * raw_mode() returns -1 if there is an error, but this should not
 * happen in this assignment.
 */

/* Returns -1 on error, 0 on success */
int raw_mode (void);
void cooked_mode (void);

#endif
