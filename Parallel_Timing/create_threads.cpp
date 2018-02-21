#include "create_threads.h"

void createThreadsVerJoin()
{
    std::cout << "============Create Threads Ver. Join============" << std::endl;

    std::thread Thread1( [](){
        std::cout << "Created a thread." << std::endl;
        for( int i = 0; i < 5; ++i ) {
            std::cout << "Thread Num. 1: HI! (" << i << ")" << std::endl;
        }
    } );
    Thread1.join();
    std::cout << "Joined a thread." << std::endl;

    std::thread Thread2;
    Thread2 = std::thread( [](){
        std::cout << "Created a thread." << std::endl;
        for( int i = 10; i < 15; ++i ) {
            std::cout << "Thread Num. 2: HI! (" << i << ")" << std::endl;
        }
    } );
    Thread2.join();
    std::cout << "Joined a thread." << std::endl;

    std::thread Thread3 = std::thread( [] ( int nParam ) {
        std::cout << "Created a thread." << std::endl;
        for( int i = 20; i < 25; ++i ) {
            std::cout << "Thread Num. 3: its parameter is (" << nParam << ")" << std::endl;
        }
    }, 4 );
    Thread3.join();
    std::cout << "Joined a thread." << std::endl;

}

void createThreadsNonJoin()
{
    std::cout << "============Create Threads Ver. non-Join============" << std::endl;

    std::thread Thread1( [](){
        std::cout << "Created a thread." << std::endl;
        for( int i = 0; i < 5; ++i ) {
            std::cout << "Thread Num. 1: HI! (" << i << ")" << std::endl;
        }
    } );

    std::thread Thread2;
    Thread2 = std::thread( [](){
        std::cout << "Created a thread." << std::endl;
        for( int i = 10; i < 15; ++i ) {
            std::cout << "Thread Num. 2: HI! (" << i << ")" << std::endl;
        }
    } );

    std::thread Thread3 = std::thread( [] ( int nParam ) {
        std::cout << "Created a thread." << std::endl;
        for( int i = 20; i < 25; ++i ) {
            std::cout << "Thread Num. 3: its parameter is (" << nParam << ")" << std::endl;
        }
    }, 4 );

    Thread1.join();
    //std::cout << "Joined a thread." << std::endl;

    Thread2.join();
    //std::cout << "Joined a thread." << std::endl;

    Thread3.join();
    //std::cout << "Joined a thread." << std::endl;

}
