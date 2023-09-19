#ifndef LASERSERVER_H
#define LASERSERVER_H

#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#include "msg2tcp/BinaryInterfaceServer.h"
#include "msg2tcp/LaserMessage.h"

class LaserServer {
  public:
    LaserServer(int port); 
    void updateLaser(const LaserMessage laser_message);
    
  private:
    int port_;
    int laser_number_;
    BinaryInterfaceServer* server_;
    
};

#endif
