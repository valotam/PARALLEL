/*
    timming.cpp -- An example application that shows
    how to use the future and async STD headers.
*/

#include "GUI.h"
#include "calcul_pi.h"
#include "create_threads.h"

int main(){
    GUI gui = GUI();
    gui.AddButton("Serial", [] {
        std::cout << "pushed button" << std::endl;
        computPI();
        });
    gui.AddButton("Parallel", [] {
        std::cout << "pushed button" << std::endl;
        createThreads();
        });

    gui.PerformLoop();
    return 0;
}
