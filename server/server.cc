

#include <httplib.h>
#include <cstdio>
#include "handlerlib.h"

#define SERVER_CERT_FILE "./cert.pem"
#define SERVER_PRIVATE_KEY_FILE "./key.pem"

using namespace httplib;

void InitSvr(Server& svr)
{
	svr.Get("/", [=](const Request& /*req*/, Response& res) {
		res.set_redirect("/hi");
	});

	svr.Get("/stop", [&](const Request& /*req*/, Response& /*res*/) {
		svr.stop();
	});

	svr.set_error_handler(Set_Error_Handler);

	svr.set_logger(Set_Logger);

	svr.Get("/hi", Hi);

	svr.Get("/slow", Slow);

	svr.Get("/dump", Dump);

	svr.Post("/fileTest", FileTest);

	svr.listen("127.0.0.1", 8080);
}

int main(void)
{
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    SSLServer svr(SERVER_CERT_FILE, SERVER_PRIVATE_KEY_FILE);
#else
    Server svr;
#endif


    if (!svr.is_valid()) {
        printf("server has an error...\n");
        return -1;
    }

	InitSvr(svr);

    return 0;
}

