/*
 * This file is released under the GNU General Public Licence
 *
 * authors:
 * Richard Ashbury       <richard.asbury@btinternet.com>
 * Jean-Christophe Hoelt <jeko@ios-software.com>
 */

#ifndef _CORONA_TYPES_H
#define _CORONA_TYPES_H

#define pause_state 10
#define normal_state 9

// TIMED LEVEL
//
struct TimedLevel
{
  unsigned char frequency[2][512];
  int state;
  int timeStamp;
  int currentTimeMs;

  int lastbeat; // filled by corona
};

// COLOR
//
struct ColorRGB 
{
  unsigned char rgbRed;
  unsigned char rgbGreen;
  unsigned char rgbBlue;
};


#endif
