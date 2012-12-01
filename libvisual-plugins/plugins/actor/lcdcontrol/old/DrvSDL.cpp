/* $Id$
 * $URL$
 *
 * Copyright (C) 2009 Scott Sibley <scott@starlon.net>
 *
 * This file is part of LCDControl.
 *
 * LCDControl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LCDControl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LCDControl.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <cstdlib>

#include "DrvSDL.h"
using namespace std;
using namespace LCD;

// LCDGraphic RealBlit
void DrvSDLBlit(LCDGraphic *lcd, const int row, const int col,
    const int height, const int width) {
    ((DrvSDL *)lcd)->DrvBlit(row, col, height, width);
}


// Constructor
DrvSDL::DrvSDL(std::string name, LCDControl *v,
    Json::Value *config, int layers) :
    LCDCore(v, name, config, LCD_GRAPHIC, (LCDGraphic *)this),
    LCDGraphic((LCDCore *)this), drvFB(0) {

    GraphicRealBlit = DrvSDLBlit;

    Json::Value *val = CFG_Fetch(config, name + ".cols", new Json::Value(SCREEN_W));
    cols_ = val->asInt();
    delete val;

    val = CFG_Fetch(config, name + ".rows", new Json::Value(SCREEN_H));
    rows_ = val->asInt();
    delete val;

    val = CFG_Fetch(config, name + ".update", new Json::Value(150));
    update_ = val->asInt();
    delete val;

    val = CFG_Fetch(config, name + ".fill", new Json::Value(0));
    fill_ = val->asInt();
    delete val;

    val = CFG_Fetch_Raw(config, name + ".pixels", new Json::Value("1x1"));
    sscanf(val->asCString(), "%dx%d", &pixels.x, &pixels.y);
    delete val;

    GraphicInit(rows_, cols_, 8, 7, layers);

    if(!(drvFB = (RGBA *)malloc(rows_ * pixels.y * cols_ * pixels.x * sizeof(RGBA))))
	LCDError("Could not allocate frame buffer for DrvSDL");

    wrapper_ = new SDLWrapper((SDLInterface *)this);

    update_thread_ = new SDLUpdateThread(this);

    //gif_thread_ = new SDLGifThread(this);

    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        LCDError("DrvSDL: Unable to init SDL: %s", SDL_GetError());
    }

    surface_ = SDL_SetVideoMode(cols_ * pixels.x, rows_ * pixels.y, 32, 
        SDL_RESIZABLE);

    SDL_WM_SetCaption("LCDControl", "LCDControl");

    if( surface_ == NULL) {
        LCDError("DrvSDL: Unable to set %dx%d video: %s", cols_, rows_, SDL_GetError());
    }

/*
    sdl_timer_.setInterval(update_);
    QObject::connect(&sdl_timer_, SIGNAL(timeout()),
        wrapper_, SLOT(DrvUpdateSDL()));
*/
}

// Destructor
DrvSDL::~DrvSDL() {
    update_thread_->wait();
    if(drvFB) {free(drvFB); drvFB = NULL;}
    delete wrapper_;
    delete update_thread_;
    SDL_Quit();
    if(gif_file_ != "" && !image_.empty()) {
        WriteGif();
    }
}

void DrvSDL::WriteGif() {
    std::list<Magick::Image> gif;

    int num = 0;
    for(std::list<Magick::Blob>::iterator it = image_.begin(); it != image_.end(); it++) {
        Magick::Image image;
        image.magick("RGB");
        image.depth(32);
        image.size(Magick::Geometry(cols_ * pixels.x, rows_ * pixels.y));
        Magick::PixelPacket *pixel_cache = image.getPixels(0, 0, cols_ * pixels.x, rows_ * pixels.y);
        for(int n = 0; n < rows_ * pixels.y * cols_ * pixels.x; n++) {
            int col = ((int *)(*it).data())[n];
            uint8_t *rgb = (uint8_t *) &col;
            Magick::PixelPacket *gif_pixel = pixel_cache + n;
            *gif_pixel = Magick::ColorRGB(rgb[0] / 256.0, rgb[1] / 256.0, rgb[2] / 256.0);
        }
        image.syncPixels();
        gif.push_back(image);
        LCDInfo("Image %d", num++);
    }
    LCDInfo("Writing GIF image...");
    for_each(gif.begin(), gif.end(), Magick::animationDelayImage(ani_speed_ / 10));
    Magick::writeImages(gif.begin(), gif.end(), gif_file_);
}

void DrvSDL::DrvUpdateGif() {
    if( SDL_MUSTLOCK(surface_))
        if(SDL_LockSurface(surface_) < 0)
            return;

    image_.push_back(Magick::Blob(surface_->pixels, rows_ * pixels.y * surface_->pitch));

    if(SDL_MUSTLOCK(surface_))
        SDL_UnlockSurface(surface_);
}

// Initialize device
void DrvSDL::SetupDevice() {
    if(update_ < 0)
        return;
    GraphicStart();
}

// Deinit driver
void DrvSDL::TakeDown() {
    Disconnect();
}

// Configuration setup
void DrvSDL::CFGSetup() {
    LCDCore::CFGSetup();
}

// Connect -- generic method called from main code
void DrvSDL::Connect() {
    DrvClear();
    update_thread_->start();
    if(gif_file_ != "") 
        gif_timer_.start();
    sdl_timer_.start();
}

// Disconnect -- deinit
void DrvSDL::Disconnect() {
}

/*void DrvSDL::DrvUpdateSDL() {

}*/

void DrvSDL::UpdateThread() {
}

void DrvSDL::DrvUpdateSDL() {
        DrvUpdateImg();
        SDL_Flip(surface_);
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {
                    case SDLK_UP:
                        emit static_cast<LCDEvents *>(visitor_->GetWrapper())->_KeypadEvent(1);
                        break;
                    case SDLK_DOWN:
                        emit static_cast<LCDEvents *>(visitor_->GetWrapper())->_KeypadEvent(2);
                        break;
                    case SDLK_TAB:
                        ToggleFullScreen();
                        break;
                    case SDLK_ESCAPE:
                        if(IsFullScreen())
                            ToggleFullScreen();
                        else 
                            goto quit;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_QUIT:
            quit:
                Disconnect();
                GetApp()->Stop();
                break;
            case SDL_VIDEORESIZE:
                LCDInfo("Resize %dx%d", event.resize.w, event.resize.h);
                Resize(event.resize.h, event.resize.w);
                break;
            default:
                break;
            }
        }
}

int DrvSDL::LockSDL() {
    if( SDL_MUSTLOCK(surface_))
        return SDL_LockSurface(surface_) < 0;
    return 0;
}

void DrvSDL::UnlockSDL() {
    if(SDL_MUSTLOCK(surface_))
        SDL_UnlockSurface(surface_);
}

void DrvSDL::DrvUpdateImg() {
    if(LockSDL() < 0)
        return;

    for(int r = 0; r < rows_; r++) {
        for(int c = 0; c < cols_; c++) {
            SDL_Rect rect;
            rect.y = r * pixels.y;
            rect.x = c * pixels.x;
            rect.h = pixels.y;
            rect.w = pixels.x;
            RGBA rgb = drvFB[r * cols_ + c];
            int col = (256 << 24) + (rgb.R << 16) + (rgb.G << 8) + rgb.B;
            SDL_FillRect(surface_, &rect, col);
        }
    }
    
    UnlockSDL();

    SDL_UpdateRect(surface_, 0, 0, cols_ * pixels.x, rows_ * pixels.y);
}

// Driver-side blit method
void DrvSDL::DrvBlit(const int row, const int col, 
    const int height, const int width) {
    mutex_.lock();
    for(int r = row; r < row + height; r++) {
        for(int c = col; c < col + width; c++) {
            drvFB[r * cols_ + c] = GraphicRGB(r, c);
        }
    }
    mutex_.unlock();
}

// Clear the LCD
void DrvSDL::DrvClear() {
    GraphicClear();
    if(fill_)
        GraphicFill();
}

int DrvSDL::IsFullScreen() {
    return (surface_->flags & SDL_FULLSCREEN) > 0;
}

void DrvSDL::ToggleFullScreen() {
    SDL_WM_ToggleFullScreen( surface_ );
    SDL_ShowCursor( IsFullScreen() ? SDL_DISABLE : SDL_ENABLE );
}

void DrvSDL::Resize(const int rows, const int cols) {
    mutex_.lock();
    RGBA *tmp = (RGBA *)realloc(drvFB, 
        rows * cols * sizeof(RGBA));
    if(!tmp) {
        LCDError("DrvSDL: Unable to reallocate buffer");
        return;
    }
    drvFB = tmp;
    emit static_cast<LCDEvents *>(visitor_->GetWrapper())->_ResizeBefore(rows, cols);
    LockSDL();
    SDL_FreeSurface(surface_);
    surface_ = SDL_SetVideoMode(cols, rows, 32, SDL_RESIZABLE);
    UnlockSDL();
    if(!surface_) {
        LCDError("Unable to resize SDL surface");
        sdl_timer_.stop();
    }
    rows_ = rows / pixels.y;
    cols_ = cols / pixels.x;
    if(visitor_->ResizeLCD(rows / pixels.y, cols / pixels.x) == 0) {
        emit static_cast<LCDEvents *>(visitor_->GetWrapper())->_ResizeAfter();
    }
    mutex_.unlock();
}

