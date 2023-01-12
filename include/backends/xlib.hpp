// MIT License
//
// Copyright (c) 2022 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//
//

#pragma once

#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>
#include <X11/XKBlib.h>
#include <cinttypes>
#include <iostream>

#define __KEYCODE_IMPL__ "xlib"
#define __CONVERT_TO_STRING__(x) XKeysymToString(x)

class value {
public:
    XID opcode = 0;
    bool is_release = false;
public:
    value() = default;
    ~value() = default;
};

class device {
public:
    Display* display;
    Window wnd;
    XIEventMask mask;

    int device_group = 0;
    int xi_opcode = 0;
public:
    device() {
        this->display = XOpenDisplay(NULL);
        this->wnd = DefaultRootWindow(this->display);
       
        this->mask.deviceid = XIAllMasterDevices;
        this->mask.mask_len = XIMaskLen(XI_LASTEVENT);
        this->mask.mask = (unsigned char*)calloc(this->mask.mask_len, sizeof(char));

        XISetMask(this->mask.mask, XI_RawKeyPress);
        XISetMask(this->mask.mask, XI_RawKeyRelease);
        XISelectEvents(this->display, this->wnd, &this->mask, 1);
        XSync(this->display, False);

        free(this->mask.mask);
    
        {
            int q_event, q_error;

            if(!XQueryExtension(this->display, "XInputExtension", &this->xi_opcode,
                    &q_event, &q_error)) {
                std::cerr << "XInputExtension not found\n";
                std::exit(1);
            }
        }

        XkbSelectEventDetails(this->display, 
                                XkbUseCoreKbd, 
                                XkbStateNotify,
                                XkbGroupStateMask,
                                XkbGroupStateMask);
        {
            XkbStateRec state;
            XkbGetState(this->display, XkbUseCoreKbd, &state);
            this->device_group = state.group;
        }

        XSelectInput(this->display, this->wnd, KeyReleaseMask | KeyPressMask);
        XMapWindow(this->display, this->wnd);
    }

    ~device() {
        XDestroyWindow(this->display, this->wnd);
    }

    value get_opcode() {
        XEvent event;
        XGenericEventCookie* cookie = (XGenericEventCookie*)&event.xcookie;
        XNextEvent(this->display, &event);

        if(XGetEventData(this->display, cookie)) {
            if(cookie->type == GenericEvent && cookie->extension == this->xi_opcode) {
                if(cookie->evtype == XI_RawKeyPress || cookie->evtype == XI_RawKeyRelease) {
                    XIRawEvent* raw_event = (XIRawEvent*)cookie->data;

                    KeySym sym = XkbKeycodeToKeysym(this->display, 
                                                raw_event->detail,
                                                this->device_group,
                                                0);

                    if(sym == NoSymbol) {
                        if(this->device_group == 0) 
                            return value();
                        else {
                            sym = XkbKeycodeToKeysym(this->display, raw_event->detail, 0, 0);

                            if(sym == NoSymbol)
                                return value();
                        }
                    }

                    XFreeEventData(this->display, cookie);
                    
                    value val;
                    val.is_release = (cookie->evtype == XI_RawKeyRelease);
                    val.opcode = sym;

                    return val;
                }
            }
        }
    }
};