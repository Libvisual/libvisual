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

#include <signal.h>
#include <QApplication>

#include "LCDControl.h"
#include "debug.h"
#include <libvisual/libvisual.h>
   
LCD::LCDControl *lcdControl;

void handler(int signal){
    LCDDebug("got signal %d", signal);
    lcdControl->Stop();
}


int main(int argc, char *argv[]) {
    verbose_level = 3;
    signal(SIGHUP, handler);
    signal(SIGINT, handler);
    signal(SIGQUIT, handler);
    signal(SIGTERM, handler);
    visual_init(&argc, &argv);
    visual_log_set_verboseness(VISUAL_LOG_DEBUG);
    QApplication *app = new QApplication(argc, argv);
    lcdControl = new LCD::LCDControl(app);
    int ret = lcdControl->Start();
    visual_quit();
    delete lcdControl;
    delete app;
    return ret;
}
