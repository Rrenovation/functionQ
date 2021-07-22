#include "functionQ.h"

int main(int argc, char *argv[])
{
    functionQ *funQ = new functionQ(argc,argv);  
    funQ->loop();
    
    return funQ->exec();
}