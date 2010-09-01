#include <stdlib.h>
#include <stdio.h>

#include "io.h"
#include "service.h"

int
main(int argc, char *argv[])
{
	/*(struct service *http_service = roe_start("http", 12345);
	http_service = http_service;*/
	roe_start("http", 9992);
	io_run();

	return 0;
}
