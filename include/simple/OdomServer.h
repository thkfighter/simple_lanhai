#ifndef ODOMSERVER_H
#define ODOMSERVER_H

#include "ros/ros.h"
#include "nav_msgs/Odometry.h"
#include "msg2tcp/BinaryInterfaceServer.h"
#include "msg2tcp/OdomMessage.h"

class OdomServer {
  public:
    OdomServer(int port); 
    void updateOdom(const OdomMessage odom_message);

  private:
    int port_;
    int odom_number_;
    BinaryInterfaceServer* server_;
   
};



#endif
