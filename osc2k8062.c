/*

	osc2k8062.c
	OSC to DMX server for the Velleman K8062/VM116 USB DMX controller.
	Based on k8062forlinux by Denis Moreaux.

	Copyright (C) 2009  Nicholas J. Humfrey
	Copyright (C) 2008  Denis Moreaux
	
	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "config.h"
#include "osc2k8062.h"

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <signal.h>

#include <usb.h>
#include <lo/lo.h>

/* Global Variables */
int channels[MAX_CHANNELS];
int channel_count = 16;
int keep_running = 1;


void write_command( usb_dev_handle *udev, unsigned char *data )
{
    usb_interrupt_write(udev, 0x1, (char*)data, 8, 20);
}

void send_dmx( usb_dev_handle *udev )
{
    int i, n;
    unsigned char data[8];

    for (i=0;(i<100) && !channels[i] && (i < channel_count - 6);i++); 
    
    data[0] = 4; /* Start of data, number of leading 0 and 6 channels */
    data[1] = i+1;
    data[2] = channels[i];
    data[3] = channels[i+1];
    data[4] = channels[i+2];
    data[5] = channels[i+3];
    data[6] = channels[i+4];
    data[7] = channels[i+5];
    write_command(udev, data);
    i+=6;
  
    while (i < channel_count - 7) {
        if (!channels[i]) {
            for(n=i+1;(n < channel_count - 6) && (n-i<100) && !channels[n] ;n++) {
                data[0] = 5;
                data[1] = n-i;
                data[2] = channels[n];
                data[3] = channels[n+1];
                data[4] = channels[n+2];
                data[5] = channels[n+3];
                data[6] = channels[n+4];
                data[7] = channels[n+5];
                write_command(udev, data);
                i=n+6;
            }
        } else {
            data[0] = 2; /* 7 channels */
            data[1] = channels[i];
            data[2] = channels[i+1];
            data[3] = channels[i+2];
            data[4] = channels[i+3];
            data[5] = channels[i+4];
            data[6] = channels[i+5];
            data[7] = channels[i+6];
            write_command(udev, data);
            i+=7;
        }
    }
    
    for(;i < channel_count;i++) {
        data[0] = 3; /* send one channel */
        data[1] = channels[i];
        write_command(udev, data);
    }
}

usb_dev_handle* init_usb()
{
  struct usb_bus *bus;
  struct usb_device *dev;
  
  usb_init();
  usb_find_busses();
  usb_find_devices();

  for (bus = usb_get_busses(); bus; bus = bus->next) {
    for (dev = bus->devices; dev; dev = dev->next) {
      if ( (dev->descriptor.idVendor == K8062_VENDOR_ID) && 
           (dev->descriptor.idProduct == K8062_PRODUCT_ID) )
      {
        usb_dev_handle *udev = usb_open(dev);
        #ifdef LIBUSB_HAS_DETACH_KERNEL_DRIVER_NP
          if (usb_detach_kernel_driver_np(udev, 0) < 0) {
              fprintf(stderr, "%s\n", usb_strerror());
              usb_close(udev);
              return NULL;
          }
        #endif
        if (usb_set_configuration(udev, 1) < 0) {
            fprintf(stderr, "%s\n", usb_strerror());
            usb_close(udev);
            return NULL;
        }
        if (usb_claim_interface(udev, 0) < 0) {
            fprintf(stderr, "%s\n", usb_strerror());
            usb_close(udev);
            return NULL;
        }
        return udev;
      }
    }
  }

  fprintf(stderr,"No K8062 DMX device found.\n");
  
  return NULL;
}


void timediff(struct timeval *res, struct timeval *a, struct timeval *b)
{
    long sec,usec;
    sec=a->tv_sec-b->tv_sec;
    usec=a->tv_usec-b->tv_usec;
    while (usec<0) {
        usec+=1000000;
        sec--;
    }
    if (sec<0) {
        res->tv_sec=0;
        res->tv_usec=0;
    } else {
        res->tv_sec=sec;
        res->tv_usec=usec;
    }
}

void timeadd(struct timeval *res, struct timeval *a, struct timeval *b)
{
    res->tv_usec=a->tv_usec+b->tv_usec;
    res->tv_sec=a->tv_sec+b->tv_sec;
    while (res->tv_usec >= 1000000) {
        res->tv_usec-=1000000;
        res->tv_sec++;
    }
}

void main_loop(usb_dev_handle *udev)
{
    struct timeval now,next,diff,delay;

    delay.tv_sec=0;
    delay.tv_usec=25000;
    gettimeofday(&next,NULL); /* First tick is now */
    while(keep_running) {
        if (channel_count<6)
            channel_count=6;
        send_dmx(udev);
        timeadd(&next,&next,&delay); /* wait to next tick */
        gettimeofday(&now,NULL);
        timediff(&diff,&next,&now); /* how much to wait */
        while (diff.tv_sec || diff.tv_usec) {
            select(0,NULL,NULL,NULL,&diff);
            gettimeofday(&now,NULL);
            timediff(&diff,&next,&now);
        };		    
    }
}

void termination_handler(int signum)
{
    if (signum==SIGINT) fprintf(stderr, "osc2k8062: Received SIGINT, exiting.\n");
    else if (signum==SIGTERM) fprintf(stderr, "osc2k8062: Received SIGTERM, exiting.\n");
    else if (signum==SIGHUP) fprintf(stderr, "osc2k8062: Received SIGHUP, exiting.\n");
    
    keep_running = 0;
}

void setup_signals()
{
    if (signal (SIGINT, termination_handler) == SIG_IGN)
        signal (SIGINT, SIG_IGN);
    if (signal (SIGHUP, termination_handler) == SIG_IGN)
        signal (SIGHUP, SIG_IGN);
    if (signal (SIGTERM, termination_handler) == SIG_IGN)
        signal (SIGTERM, SIG_IGN);
}

void error(int num, const char *msg, const char *path)
{
    fprintf(stderr, "liblo server error %d in path %s: %s\n", num, path, msg);
}

int osc_set_handler(const char *path, const char *types, lo_arg **argv, int argc,
		 void *data, void *user_data)
{
    int channel = 0;
    int value = argv[0]->f * 255;
    if (sscanf(path, "/dmx/%d/set", &channel)) {
        printf("Setting %d to %d.\n", channel, value);
        channels[channel] = value;
        return 0;
    } else {
        fprintf(stderr, "sscanf failed to match: %s\n", path);
        return 1;
    }
}

lo_server_thread start_server(char* port)
{
    lo_server_thread st = lo_server_thread_new(port, error);
    int i;
    
    printf("Started server on port %d.\n", lo_server_thread_get_port(st));
    
    /* Add the callbacks for each of the channels */
    for(i=0;i<channel_count;i++) {
        char path[16];
        snprintf(path, 15, "/dmx/%d/set", i);
        lo_server_thread_add_method(st, path, "f", osc_set_handler, NULL);
    }
    
    /* Start the thread */
    lo_server_thread_start( st );

    return st;
}

int main() {
    usb_dev_handle *udev = init_usb();
    lo_server_thread st = NULL;
    int i;
    
    /* Did we setup USB ok? */
    if (udev == NULL) return -1;

    /* Initialise all channels to 0 */
    for(i=0;i<MAX_CHANNELS;i++) channels[i] = 0x00;

    /* OSC server failed? */
    st = start_server("7770");
    if (st == NULL) {
        fprintf(stderr,"Failed to start OSC server thread.\n");
        return -1;
    }

    /* Setup signal handling - so we exit cleanly */
    setup_signals();

    /* Run the main loop */
    main_loop( udev );

    /* Set all channels to 0 */
    for(i=0;i<MAX_CHANNELS;i++) channels[i] = 0x00;
    channel_count=MAX_CHANNELS;
    send_dmx(udev);

    /* Clean up */
    usb_close(udev);
    lo_server_thread_stop(st);
    lo_server_thread_free(st);
    
    return 0;
}
