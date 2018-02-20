/*
    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
#include <nanogui/nanogui.h>

#ifndef GUI_H
#define GUI_H

using namespace nanogui;

class GUI
{
    public:
        GUI();
        virtual ~GUI();

        unsigned int GetCounter() { return m_Counter; }
        void SetCounter(unsigned int val) { m_Counter = val; }

        void PerformLoop();
        void AddButton(const std::string &lable, const std::function<void()> &cd);

    protected:

    private:
        unsigned int m_Counter;
        Screen *screen = nullptr;
        FormHelper *gui;
        ref<Window> window;

};

#endif // GUI_H
