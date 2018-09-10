#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Joy.h>
#include <std_msgs/Float64.h>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <chrono>

using std::cout;
float bit1,bit2;
float last_error=0,targetp;
//30,40
float kp=20,ki=0.0,kd=0.0,integral=0,tau=0,pid_d=0,pid_i=0,pid_p=0;
geometry_msgs::Twist twist;
std_msgs::Float64 errp,angle;
float tmax=200,tmin=-200,T_prv=0,T_prs,trq;
time_t start,end;

#include <sstream>

void strCallback(const std_msgs::Float64::ConstPtr& float_msgs) 
{  bit1 = float_msgs->data;
ROS_INFO("I heard:[%f]",bit1);
 }

void strCallback2(const std_msgs::Float64::ConstPtr& float_msgs) 
{  bit2 = float_msgs->data;
ROS_INFO("I heard: [%f]", bit2);
 }
void angCallback2(const std_msgs::Float64::ConstPtr& float_msgs) 
{  targetp = float_msgs->data;
 }



int main(int argc, char **argv)
{

  ros::init(argc, argv, "oh_hi");
  ros::NodeHandle n;
   ros::NodeHandle prive("~");

	ros::Publisher vel = n.advertise<geometry_msgs::Twist>("joy_vel", 15);
	ros::Publisher err = n.advertise<std_msgs::Float64>("error", 500);
	ros::Publisher tar = n.advertise<std_msgs::Float64>("target", 500);
	ros::Publisher ang = n.advertise<std_msgs::Float64>("angle", 500);
	ros::Subscriber sub = n.subscribe("can_st_bit1", 500, strCallback);
	ros::Subscriber sub2 = n.subscribe("can_st_bit2", 500, strCallback2);
	ros::Subscriber sub3 = n.subscribe("ang2go", 500, angCallback2);
	ros::Rate loop_rate(10);
	int count = 0;

	ros::spinOnce();
	float bit2val,str_ang;
	
	//reference from previous code
    //twist.angular.x = st_13 + k/(.002857*1.5);
	//twist.angular.y = st_12 - k/(.002857*1.5);
	tmax=500+targetp;
	tmin=-500-targetp;
	
	while (ros::ok()) 
	{	auto start = std::chrono::high_resolution_clock::now();
		if(bit2>21)
		{
		bit2val=255-bit2;
		str_ang=((bit2val*255+(255-bit1))/10);
		}
		else
		{
		bit2val=bit2;
		str_ang=-((bit2val*255+bit1))/10;
		}
		prive.getParam("can_kp", kp);
		prive.getParam("can_ki", ki);
		prive.getParam("can_kd", kd);
		float currentp,derivative,torque,error;
		int st_13=3088; 
		
		currentp=str_ang;
		error=(targetp - currentp);
		pid_p=kp*error;
		
		if(abs(error) < 5)
		{
			
		pid_i=pid_i + (ki*error);
		}
		auto finish = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = finish - start;
		double dif=elapsed.count();

		pid_d= ki*((error-last_error)/(dif*1000));

		torque = pid_p + pid_i + pid_d;

		if(torque>tmax)
		{
		trq=tmax;
		}
		else if(torque<tmin)
		{
		trq=tmin;
		}
		else
		{
		trq=torque;
		}
		//trq=500;
		//tprs=torque;
		//trq=(torque)+tau*(tprs-tprv);
		//tprv=trq;
		
		twist.angular.x = st_13+(trq);
		twist.angular.y = st_13-(trq);
		errp.data=error;
		angle.data=dif;
		vel.publish(twist);
		err.publish(errp);

		ang.publish(angle);
		ros::spinOnce();
		loop_rate.sleep();
		++count;
		last_error=error;
	}

	return 0;
}
















  



