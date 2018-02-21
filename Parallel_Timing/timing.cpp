/*
    timming.cpp -- An example application that shows
    how to use the future and async STD headers.
*/

#include "GUI.h"
#include "calcul_pi.h"
#include "create_threads.h"

int main(){
    GUI gui = GUI();

    gui.AddButton("Create threads ver. random", [] {
        createThreadsNonJoin();
        });

    gui.AddButton("Create threads ver. join", [] {
        createThreadsVerJoin();
        });

    gui.AddButton("Compare the performance of serial and parallel programs", [] {
        computPI();
        });

    gui.PerformLoop();
    return 0;
}
