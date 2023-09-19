
//According to data struct 
struct __attribute__ ((packed)) OdomMessage
{
  double timestamp; //timestamp (UTC [sec]) of corr. scan
  uint32_t odom_number;
  uint64_t epoch;
  double x;
  double y;
  double yaw;
  double v_x;
  double v_y;
  double omega;
  bool velocity_set;
};
