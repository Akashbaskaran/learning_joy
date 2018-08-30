#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Joy.h>
#include <std_msgs/Float64.h>
float err,curr_ang; int count=0;
std_msgs::Float64 ang_go;
float tar_ang=60,fin_ang;
#include <sstream>

void errCallback(const std_msgs::Float64::ConstPtr& float_msgs) 
{  err = float_msgs->data;
 }
void angCallback(const std_msgs::Float64::ConstPtr& float_msgs) 
{  curr_ang = float_msgs->data;
 }

int main(int argc, char **argv)
{

  ros::init(argc, argv, "anggo");
  ros::NodeHandle n;

	ros::Publisher angl = n.advertise<std_msgs::Float64>("ang2go", 500);
	ros::Subscriber sub3 = n.subscribe("error", 500, errCallback);
	ros::Subscriber sub4 = n.subscribe("angle", 500, angCallback);
	ros::Rate loop_rate(10);
	
	ros::spinOnce();
	float bit2val,str_ang;
	
	while (ros::ok()) 
	{	
		float prev_ang;
		if (count==0)
		{fin_ang=curr_ang+10; count++;
		}

		if ((err<3) && ((tar_ang - fin_ang)>3))
		{
		fin_ang=curr_ang+10;
		}

		if (tar_ang - curr_ang <2 && tar_ang< 230)
		{tar_ang=tar_ang+80;
			}
		

		ang_go.data=fin_ang;
		angl.publish(ang_go);
		
		ros::spinOnce();
		loop_rate.sleep();
		++count;
	}

	return 0;
}
















  



