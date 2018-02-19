/*
    timing.cpp -- An example application that shows
    how to use the future and async STD headers.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
#include <nanogui/nanogui.h>

#include <iostream>
#include <future>

using namespace nanogui;

int main(int /* argc */, char ** /* argv */) {
    nanogui::init();

    /* scoped variables */ {
        bool use_gl_4_1 = false;// Set to true to create an OpenGL 4.1 context.
        Screen *screen = nullptr;

        if (use_gl_4_1) {
            // NanoGUI presents many options for you to utilize at your discretion.
            // See include/nanogui/screen.h for what all of these represent.
            screen = new Screen(Vector2i(500, 700), "NanoGUI test [GL 4.1]",
                                /*resizable*/true, /*fullscreen*/false, /*colorBits*/8,
                                /*alphaBits*/8, /*depthBits*/24, /*stencilBits*/8,
                                /*nSamples*/0, /*glMajor*/4, /*glMinor*/1);
        } else {
            screen = new Screen(Vector2i(500, 700), "NanoGUI test");
        }


        /// dvar, bar, strvar, etc. are double/bool/string/.. variables
        FormHelper *gui = new FormHelper(screen);
        ref<Window> window = gui->addWindow(Eigen::Vector2i(10, 10), "Form helper example");

        gui->addButton("A button", []() { std::cout << "Button pressed." << std::endl; });
        gui->addButton("Two button", []() { std::cout << "Button pressed." << std::endl; });

        screen->setVisible(true);
        screen->performLayout();
        //window->center();

        nanogui::mainloop();
    }
    nanogui::shutdown();
    return 0;
}
