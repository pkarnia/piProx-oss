// printhex, example userspace code for interfacing with piProx and decoding the HID Corporate 1000 format
// Copyright (C) 2017 Nash Kaminski
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.
#include "libpiprox.h"
int main(int argc, char **argv){
    const char devnode[] = "/dev/prox";
    piprox_state_t prox;
    piprox_hidH10301_t hidcorp1k;
    int rv;
    /* open the device node and initialize the internal state */
    if(argc > 1){
        rv = piprox_open(&prox, argv[1]);
    }
    else{
        rv = piprox_open(&prox, devnode);
    }
    if(rv < 0){
        fprintf(stderr, "piprox_open failed, return code is %d\n", rv);
        return 1;
    }
    
    while(1){
        printf("Waiting for card...\n");
        /* Read a card */
        rv = piprox_read(&prox);
        printf("Read %d bytes from reader\n", rv);
        if(rv > 0){
            /* If the read was successful, print the data and attempt a corp1k decode */
            piprox_print(&prox, STDOUT_FILENO);
            rv = piprox_hidH10301_parse(&prox, &hidcorp1k);
            if(rv == 0){
                printf("Card data is smaller than 5 bytes. Attempting H10301 decode.\n");
	    	piprox_hidH10301_t hidH10301;
	    	rv = piprox_hidH10301_parse(&prox, &hidH10301);
		if (rv == 0){
			printf("Card data is smaller than 4 bytes.\n");
		} else if (rv < 0){
			printf("Parity error on parity check %d in HID H10301 decoding\n", -rv);
		}
     		else {
			printf("HID H10301 decoding done, facility=%d cardnum=%d\n", hidH10301.facility, hidH10301.cardnum);
		}
	    } else if(rv < 0){
                printf("Parity error on parity check %d in HID Corp. 1000 decoding\n", -rv);
            } else{
                printf("HID Corporate 1000 decoding done, facility=%d cardnum=%d\n", hidcorp1k.facility, hidcorp1k.cardnum);
            }
        }
        else {
	    return 1;
	}
    }
return 0;
}
