#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Joy.h>
#include <std_msgs/Float64.h>
float angle_fin,fin_ang; 
std_msgs::Float64 ang_2steer;

#include <sstream>

void angCallback(const std_msgs::Float64::ConstPtr& float_msgs) 
{  angle_fin = float_msgs->data;
 }

int main(int argc, char **argv)
{

  ros::init(argc, argv, "angsteer");
  ros::NodeHandle n;

	ros::Publisher anglo = n.advertise<std_msgs::Float64>("ang2steer", 500);
	ros::Subscriber sub4 = n.subscribe("angle", 500, angCallback);
	ros::Rate loop_rate(10);
	
	ros::spinOnce();
	float bit2val,str_ang;
	
	while (ros::ok()) 
	{			
		fin_ang=angle_fin;
		ang_2steer.data=fin_ang;
		anglo.publish(ang_2steer);
		
		ros::spinOnce();
		loop_rate.sleep();
	}

	return 0;
}
















  



