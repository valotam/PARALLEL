#include "create_threads.h"

void createThreads()
{
    std::thread Thread1( [](){
        for( int i = 0; i < 5; ++i ) {
            std::cout << "Thread Num : " << i << std::endl;
        }
    } );
    Thread1.join();

    std::thread Thread2;
    Thread2 = std::thread( [](){
        for( int i = 10; i < 15; ++i ) {
            std::cout << "Thread Num : " << i << std::endl;
        }
    } );
    Thread2.join();

    std::thread Thread3 = std::thread( [] ( int nParam ) {
        for( int i = 20; i < 25; ++i ) {
            std::cout << "Thread Parameter : " << nParam << std::endl;
        }
    }, 4 );
    Thread3.join();
}
