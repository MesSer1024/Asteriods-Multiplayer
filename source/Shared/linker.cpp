#include <Shared/Network/NetworkTypes.h>

int dummy() { return 42; }

int foobar()
{
	asteroids::net::init();

	asteroids::net::Socket* foo = nullptr;
	asteroids::net::socket_create(foo);

	return 12;
}