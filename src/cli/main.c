#include <stdio.h>
#include <stdlib.h>

#include "../inc/cli/main.h"
#include "common.h"
#include "args.h"

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    printf("Client\n");
    
    main_client();
    common();
    
    return 0;
}

void main_client(void){
    printf("Main client\n");
}
