
#ifndef __DRV_SDL_H__
#define __DRV_SDL_H__

#include <string>
#include <list>
#include <vector>
#include <SDL.h>
#include <Magick++.h>
#include <sys/time.h>


#include "LCDGraphic.h"
#include "LCDCore.h"
#include "LCDControl.h"
#include "RGBA.h"
#include "debug.h"

#define SCREEN_H 64
#define SCREEN_W 256

namespace LCD {

class SDLUpdateThread;

class DrvSDL : public LCDCore, public LCDGraphic,
    public SDLInterface {

    SDL_Surface *surface_;

    SDLWrapper *wrapper_;
    SDLUpdateThread *update_thread_;
    RGBA *drvFB;
    //std::vector<RGBA> drvFB;

/*
    QMutex mutex_;
    QTimer gif_timer_;
    QTimer sdl_timer_;
*/

    std::list<Magick::Blob> image_;

    bool connected_;
    int update_;
    int cols_;
    int rows_;
    std::string gif_file_;
    int ani_speed_;
    int fill_;
    struct _pixels { int y; int x; } pixels;
    timeval gif_last_update_;

    void DrvClear();
    void DrvUpdateImg();
    void DrvUpdate();

    public:
    DrvSDL(std::string name, LCDControl *v,
        Json::Value *config, int layers);
    ~DrvSDL();    
    void SetupDevice();
    void TakeDown();
    void CFGSetup();
    void Connect();
    void Disconnect();
    void UpdateThread();
    void DrvUpdateGif();
    void DrvUpdateSDL();
    int LockSDL();
    void UnlockSDL();
    void WriteGif();
    int IsFullScreen();
    void ToggleFullScreen();
    void Resize(const int rows, const int cols);
    void DrvBlit(const int row, const int col, const int height, const int width);

};

/*
class SDLUpdateThread : public QThread {
    Q_OBJECT
    DrvSDL *visitor_;

    protected:
    void run() { visitor_->UpdateThread(); }

    public:
    SDLUpdateThread(DrvSDL *v) { visitor_ = v; }
};
*/
}; // End namespace

#endif
