#include "GUI.h"

GUI::GUI()
{
    nanogui::init();

    /* scoped variables */
    bool use_gl_4_1 = false;// Set to true to create an OpenGL 4.1 context.

    if (use_gl_4_1) {
        // NanoGUI presents many options for you to utilize at your discretion.
        // See include/nanogui/screen.h for what all of these represent.
        screen = new Screen(Eigen::Vector2i(500, 700), "NanoGUI test [GL 4.1]",
                            /*resizable*/true, /*fullscreen*/false, /*colorBits*/8,
                            /*alphaBits*/8, /*depthBits*/24, /*stencilBits*/8,
                            /*nSamples*/0, /*glMajor*/4, /*glMinor*/1);
    } else {
        screen = new Screen(Eigen::Vector2i(500, 700), "NanoGUI test");
    }
    screen->setVisible(true);


    /// dvar, bar, strvar, etc. are double/bool/string/.. variables
    gui = new FormHelper(screen);
    window = gui->addWindow(Eigen::Vector2i(10, 10), "Comput PI");

    //window->center();
}

GUI::~GUI()
{
    nanogui::shutdown();
}


void GUI::PerformLoop()
{
    screen->performLayout();

    nanogui::mainloop();
}

void GUI::AddButton(const std::string &lable, const std::function<void()> &cd)
{
    gui->addButton(lable, cd);
}
