
#ifndef __DRV_NIFTY_H__
#define __DRV_NIFTY_H__

#include <string>
#include <libvisual/libvisual.h>

#include "LCDGraphic.h"
#include "LCDCore.h"
#include "LCDControl.h"
#include "RGBA.h"
#include "debug.h"

#define SCREEN_H 64
#define SCREEN_W 256

namespace LCD {

class DrvVideo : public LCDCore, public LCDGraphic {

    VisVideo *video_;

    bool connected_;
    int update_;
    int cols_;
    int rows_;
    int bpp_;
    int depth_;

    void DrvClear();
    void DrvUpdateImg();
    void DrvUpdate();

    public:
    DrvVideo(std::string name, LCDControl *v,
        Json::Value *config, int layers, VisEventQueue *eventqueue);
    ~DrvVideo();    
    void SetupDevice();
    void TakeDown();
    void CFGSetup();
    void Connect();
    void Disconnect();
    void UpdateThread();
    void DrvBlit(const int row, const int col, const int height, const int width);

};


}; // End namespace

#endif
