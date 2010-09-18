#include <stdlib.h>
#include <stdio.h>

#include "io.h"
#include "services.h"

int
main(int argc, char *argv[])
{
	/*(struct service *http_service = roe_start("http", 12345);
	http_service = http_service;*/
	roe_start("http", 9991);
	io_run();

	return 0;
}
