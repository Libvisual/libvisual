/*
 * This file is released under the GNU General Public Licence
 *
 * authors:
 * Richard Ashbury       <richard.asbury@btinternet.com>
 * Jean-Christophe Hoelt <jeko@ios-software.com>
 */

/////////////////////////////////////////////////////////////////////////////
//
// corona.h : Declaration of the Corona class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __CORONA_H_
#define __CORONA_H_

#include "corona_types.h"

// presets
enum {
  PRESET_CORONA = 0,
  PRESET_BLAZE,
  PRESET_COUNT
};

// The particles
struct Particle
{
  double x, y, xvel, yvel;
};

// The background swirl
struct Swirl
{
  double x;
  double y;
  double tightness;
  double pull;
};


/////////////////////////////////////////////////////////////////////////////
//
// Core Class of the Corona Visual FX
//
/////////////////////////////////////////////////////////////////////////////

class Corona
{
  private:
    int m_clrForeground;    // foreground color
    int m_nPreset;

    Particle *m_particles;
    int       nbParticules;

    // The off-screen buffer
    unsigned char* m_image;
    unsigned char* m_real_image;

    int m_width;
    int m_height;
    int m_real_height;

    Swirl m_swirl;
    unsigned char** m_deltafield;

    // Particle movement info
    int   m_swirltime;
    Swirl m_movement;

    bool m_testing;
    bool m_silent;

    // Beat detection
    double m_avg;
    double m_oldval;
    int    m_pos;

    // Waves
    double m_waveloop;
    int   *m_reflArray;
    
    // Implementation functions
    double random(double min, double max) const;
    //    void setUpPalette();
    void drawLine(int x0, int y0, int x1, int y1, unsigned char col);
    void drawParticules();
    void drawParticulesWithShift();
    void blurImage();
    void drawReflected();
    
    void chooseRandomSwirl();
    void setPointDelta(int x, int y);
    void applyDeltaField(bool heavy);
    int  getBeatVal(TimedLevel *tl);
    void getAvgParticlePos(double& x, double& y) const;
    void genReflectedWaves(double loop);


  public:
    Corona();
    ~Corona();

    bool           setUpSurface(int width, int height);
    void           update(TimedLevel *pLevels);
    unsigned char *getSurface() const { return m_real_image; }

    int  getWidth()  const { return m_width;  }
    int  getHeight() const { return m_real_height; }
};

#endif //__CORONA_H_
