#ifndef _ONVIF_API_H
#define _ONVIF_API_H

#include <stdio.h>

/*
 * @brief	Below API will initialize and read the Onvif configuration parameters. 
		The UDP sockets required for the discovery protocols are initialized. 
		The failure in initialization will make sure the ONVIF TCP requests are 
		also not processed.
 * @param	NULL
 * @retval	0: Onvif configuration is initialized and discovery protocol initialization is successfull
 * @retval	-1: Error in reading configuration file/ error in initializing discovery protocols 
 */

int onvifInit(void);

/*
 * @brief	Below API will process all the incoming ONVIF requests. The below API
		will not process the non-ONVIF requests. The request will return an error
		only if there is an error in processing an ONVIF request. This error can 
		arise due to failure in initializing the TCP socket
 * @param	buf  - incoming request stream
		fd   - TCP socket descriptor
		status - Request status
 * @retval	0: Successfully processed the request
 * @retval	-1: Error in processing the request
 */

int onvif_process_request(char *buf, int fd, int status);

#endif


