/*
 *  Copyright (C) 2009 Nicholas J. Humfrey
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <lo/lo.h>


static
void usage( )
{
	printf("dmx-sender version %s\n\n", PACKAGE_VERSION);
	printf("Usage: dmx-sender [options] <channel> <value>\n");
	printf("   -u <url>    URL for remote MadJACK server\n");
	printf("   -p <port>   Port for remote MadJACK server\n\n");
	exit(1);
}



int main(int argc, char *argv[])
{
	char *port = NULL;
	char *url = NULL;
	lo_address addr = NULL;
	int result = -1;
	int opt;

	// Parse Switches
	while ((opt = getopt(argc, argv, "p:u:h")) != -1) {
		switch (opt) {
			case 'p':
				port = optarg;
				break;
				
			case 'u':
				url = optarg;
				break;
				
			default:
				usage( );
				break;
		}
	}
	
	// Need either a port or URL
	if (!port && !url) {
		fprintf(stderr, "Either URL or Port argument is required.\n");
		usage( );
	}
	
	// Check remaining arguments
  argc -= optind;
  argv += optind;
  if (argc<2) usage( );
    
    
	// Create address structure to send on
	if (port) 	addr = lo_address_new(NULL, port);
	else		addr = lo_address_new_from_url(url);


  result = lo_send(addr, "/dmx/set", "if", atoi(argv[0]), atof(argv[1]));
	if (result<1) {
		fprintf(stderr, "Error: failed to send OSC message (error=%d).\n", result);
		return -1;
	}
	
	lo_address_free( addr );

    return 0;
}


