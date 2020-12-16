#include <Shared/Network/NetworkTypes.h>

int dummy() { return 42; }

int foobar()
{
	asteroids::Net::init();

	asteroids::Net::Socket* foo = nullptr;
	asteroids::Net::socket_create(foo);

	return 12;
}