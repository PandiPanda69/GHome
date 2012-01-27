#ifndef __IOS_ERROR_H__
#define __IOS_ERROR_H__

enum {
        IOS_OK = 0,
	IOS_ERROR = -1,
        IOS_UNKNOWN_DRIVER = -2,
	IOS_TOO_MANY_DRIVER_PLUGGED = -3,
	IOS_INVALID_DRIVER = -4,
	IOS_TOO_MANY_DEVICES_PLUGGED = -5,
	IOS_INVALID_DEVICE_ID = -6,
	IOS_INVALID_PORT = -7,
	IOS_UNKNOWN_DEVICE = -8,
	IOS_UNABLE_TO_INIT_DRIVER = -9,
	IOS_INVALID_MAJOR = -10
};


#endif
