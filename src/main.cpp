#include <ros/ros.h>
#include <signal.h>
#include <termios.h>
#include <stdio.h>
#include <std_msgs/Float32.h>

#include <iostream>

using namespace std; 

#define KEYCODE_R 0x43 
#define KEYCODE_L 0x44
#define KEYCODE_U 0x41
#define KEYCODE_D 0x42
#define KEYCODE_Q 0x71

class TeleopCar
{
public:
  TeleopCar();
  void keyLoop();

private:

  ros::NodeHandle nh;
  std_msgs::Float32 speed, steer;
  ros::Publisher speed_pub;
  ros::Publisher steer_pub;
};

TeleopCar::TeleopCar()
  
{
  speed.data = 0;
  steer.data = 0;
  speed_pub = nh.advertise<std_msgs::Float32>("/set_speed_car_api", 10);
  steer_pub = nh.advertise<std_msgs::Float32>("/set_steer_car_api", 10);
}

int kfd = 0;
struct termios cooked, raw;

void quit(int sig)
{
  (void)sig;
  tcsetattr(kfd, TCSANOW, &cooked);
  ros::shutdown();
  exit(0);
}


int main(int argc, char** argv)
{
  ros::init(argc, argv, "car_control_console");
  TeleopCar teleop_car;

  signal(SIGINT,quit);

  teleop_car.keyLoop();
  
  return(0);
}
int decode(char c){
  switch(c)
    {
      case KEYCODE_L:
        ROS_DEBUG("LEFT");
        puts("+ left");        
        break;
      case KEYCODE_R:
        ROS_DEBUG("RIGHT");
        puts("+ right");        
        break;
      case KEYCODE_U:
        ROS_DEBUG("UP");
        puts("+ up");       
        break;
      case KEYCODE_D:
        ROS_DEBUG("DOWN");
        puts("+ down");        
        break;
    }
}
void TeleopCar::keyLoop()
{
  char c;
  char d;

  // get the console in raw mode                                                              
  tcgetattr(kfd, &cooked);
  memcpy(&raw, &cooked, sizeof(struct termios));
  raw.c_lflag &=~ (ICANON | ECHO);
  // Setting a new line, then end of file                         
  raw.c_cc[VEOL] = 1;
  raw.c_cc[VEOF] = 2;
  tcsetattr(kfd, TCSANOW, &raw);

  puts("Reading from keyboard");
  puts("---------------------------");
  puts("Use arrow keys to move the turtle.");

  steer.data = speed.data = 0;

  for(;;)
  {
    // get the next event from the keyboard  
    if(read(kfd, &c, 1) < 0)
    {
      perror("read():");
      exit(-1);
    }

    ROS_DEBUG("value: 0x%02X\n", c);
  
    switch(c)
    {
      case KEYCODE_L:
        ROS_DEBUG("LEFT");
        steer.data = 40;
        puts("left"); 
        break;
      case KEYCODE_R:
        ROS_DEBUG("RIGHT");
        steer.data = -40;
        puts("right");        
        break;
      case KEYCODE_D:
        ROS_DEBUG("DOWN");
        speed.data = 0.0;
        puts("down");        
        break;
      case KEYCODE_U:
        ROS_DEBUG("UP");
        speed.data = 15.0;
        puts("up");        
        break;
    }
   
    speed_pub.publish(speed);
    steer_pub.publish(steer);
  }


  return;
}
