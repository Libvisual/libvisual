/*
 * This file is released under the GNU General Public Licence
 *
 * authors:
 * Richard Ashbury       <richard.asbury@btinternet.com>
 * Jean-Christophe Hoelt <jeko@ios-software.com>
 */

#ifndef _PALETTE_H
#define _PALETTE_H

#define NB_PALETTES 23

#include <libvisual/libvisual.h>

#include "corona_types.h"

// PALETTE
// 
typedef ColorRGB Palette[256];
class   CompressedPalette;

// COLLECTION OF PALETTES
//
// Definition of some palettes stored in compressed format.
//
class PaletteCollection
{
  public:
    PaletteCollection(const int palettes[][NB_PALETTES], int nbPalettes);
    ~PaletteCollection();

    int size() const  { return m_nbPalettes; }
    void expandPalette(int i, Palette dest) const;

  private:
    CompressedPalette *m_cpal;
    int                m_nbPalettes;
};

// PALETTE CYCLER
//
// A class to create cycling palettes.
// 
class PaletteCycler
{
  private:
    Palette m_srcpal;
    Palette m_destpal;
    Palette m_curpal;

    PaletteCollection m_palettes;
    int    m_srcnum, m_destnum;
    bool   m_transferring;
    double m_progress;

    void startPaletteTransition();
    void affectPaletteTransition(double p);

  public:
    PaletteCycler(const int palettes[][NB_PALETTES], int nbPalettes);
    void update(TimedLevel *pLevels);
    void updateVisPalette(VisPalette *pal);
    const Palette &getPalette() const { return m_curpal; }
};

// PALETTE CONVERSIOn
//
// Display/Architecture specific routines to convert Palette to int[] (hardware color)
// 
void paletteToRGBA(int dest[256], const Palette src);

// BLIT A PALETTIZED SURFACE using converted palette.
void blitSurface8To32(unsigned char *byteSurf, int *colorSurf, int palette[256], int size);

#endif
