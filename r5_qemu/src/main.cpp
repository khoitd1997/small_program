#include <sleep.h>
#include "xil_printf.h"

#include "lib_src.h"
#include "lwip/init.h"

int main() {
    // lwip_init();

    xil_printf("1\n\r");
    xil_printf("2\n\r");
    xil_printf("3\n\r");
    xil_printf("4\n\r");

    while(1) {

    }

    return 0;
}