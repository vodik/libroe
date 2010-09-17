

static void
ws_respond(IO *io, struct request *request)
{
	struct string *response;
	static const char template[] =
		"HTTP/1.1 %d %s\r\n"
		"Upgrade: WebSocket\r\n"
		"Connection: Upgrade\r\n"
		"WebSocket-Origin: %s\r\n"
		"WebSocket-Location: ws://%s%s\r\n"
		"WebSocket-Protocol: sample\r\n";

	char *host = hashtable_get(&request->headers, "Host");
	char *origin = hashtable_get(&request->headers, "Origin");
	char *path = request->path;

	assert(host);
	assert(origin);
	assert(path);

	response = string_new(0);
	string_catf(response, template, 101, "Web Socket Protocol Handshake", origin, host, path);
	string_cat(response, "\r\n");

	io_write(io, string_raw(&response), string_len(&response));

	string_free(response);
	return 0;
}

struct conn *
conn_new_ws(IO *io)
{
	struct request *reqest = parser_request(io);

	if (hashtable_get(request->headers, "Sec-WebSocket-Key1") || hashtable_get(request->headers, "Sec-WebSocket-Key2")) {
		printf("formal websocket handshake supported\n");
		exit(EXIT_FAILURE);
	}

	ws_respond(io->request);

	struct conn *conn = malloc(sizeof(struct conn));
	conn->io = io;

	return conn;
}

size_t
ws_write(struct conn *conn, const char *buf, size_t len)
{
	char _buf[len + 2];
	char *b = _buf;
	int i;

	*b++ = (char)0x00;
	for (i = 0; i < nbytes; ++i)
		*b++ = *buf++;
	*b = (char)0xff;

	return io_write(conn->io, _buf, len + 2);
}

void
ws_read(struct conn *conn, char *buf, size_t len)
{
	// TODO: read, this isn't a straight read,
	// a whole message needs to be pulled out, not
	// just nbytes
	
	//char buf[len + 2];
	//io_read(conn->io, buf, len);
	// TODO: parse
}

void
ws_watch(struct conn *conn)
{
}
