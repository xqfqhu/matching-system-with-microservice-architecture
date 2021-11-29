#include "util.h"


int main(int argc, char ** argv){
    /* sanity check */
    if (argc != 2){
        fprintf(stderr, "usage: %s <username>\n", argv[0]);
        exit(1);
    }

    /* run client routines */
    client(LOCALHOST, PORT_C_SERVE_A, argv[1], false);
    return 0;
    
}

