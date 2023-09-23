#ifndef LIDARWEBSERVICE_H
#define LIDARWEBSERVICE_H
#include"data.h"
#include<iostream>
extern "C"
{
	#include"third_party/cJson/cJSON.h"
}

class LidarWebService
{
public:
	LidarWebService(int port);
	~LidarWebService();
	void  run(int lidarID);
	void  stop();

private:
	
	int m_port;

};
#endif //LIDARWEBSERVICE_H