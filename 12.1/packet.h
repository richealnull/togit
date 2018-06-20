#ifndef __PACKET_H__
#define __PACKET_H__

#include <assert.h>

class Packet
{
private:
	int _ref;
public:
	Packet()
	{
		_ref	= 0;
	}
	virtual ~Packet() {}

	virtual unsigned char *data()	= 0;
	virtual unsigned int length()	= 0;
	void addRef() {_ref++;}
	void release()
	{
		assert(_ref > 0);

		_ref--;
		if(_ref <= 0)
			delete this;
	}
};

#endif
