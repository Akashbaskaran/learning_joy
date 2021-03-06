#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Joy.h>
#include <std_msgs/Float64.h>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <chrono>
#include <sensor_msgs/CameraInfo.h>

using std::cout;
float bit1,bit2;
float last_error=0,targetp;
//30,40
float kp=0,ki=0.0,kd=0.0,integral=0,tau=0,pid_d=0,pid_i=0,pid_p=0;
geometry_msgs::Twist twist;
sensor_msgs::CameraInfo str_inf;
std_msgs::Float64 errp,angle;
float tmax=300,tmin=-300,T_prv=0,T_prs,trq;
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

	ros::Publisher vel = n.advertise<geometry_msgs::Twist>("joy_vel", 1);
	ros::Publisher err = n.advertise<std_msgs::Float64>("error", 500);
	ros::Publisher tar = n.advertise<std_msgs::Float64>("target", 500);
	ros::Publisher can_info = n.advertise<sensor_msgs::CameraInfo>("can_str", 500);
	ros::Publisher ang = n.advertise<std_msgs::Float64>("angle", 500);
	ros::Subscriber sub = n.subscribe("can_st_bit1", 500, strCallback);
	ros::Subscriber sub2 = n.subscribe("can_st_bit2", 500, strCallback2);
	ros::Subscriber sub3 = n.subscribe("ang2steer", 500, angCallback2);
	ros::Rate loop_rate(10);
	int count = 0;

	ros::spinOnce();
	float bit2val,str_ang;
	
	//reference from previous code
   	//twist.angular.x = st_13 + k/(.002857*1.5);
	//twist.angular.y = st_12 - k/(.002857*1.5);
	tmax=500+abs(targetp);
	tmin=-500-abs(targetp);
	
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

		
		float currentp,derivative,torque,error;
		int st_13=3088; 
		
		currentp=str_ang;
		error=(targetp - currentp);
		
		prive.getParam("can_kp", kp);
		prive.getParam("can_ki", ki);
		prive.getParam("can_kd", kd);
		// New code with over shoot 
		// if (abs(error) > 5)
		// {
		// pid_p=kp*error;
		// pid_i=ki*error + pid_i;
		// torque=pid_p +pid_i;
		// }
		// else
		// {
		// 	torque=0;
		// 	pid_i=0;
		// }


		// Old Code
		// pid_p=kp*error;
		// if(abs(error)>5)
		// {	
	
		// pid_i=pid_i + (ki*error);
		// }
		// else
		// 	{ 
		// 	pid_i=0;
		// 	}


		pid_p=kp*error;
		
		pid_i=ki*error + pid_i;
		auto finish = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = finish - start;
		double dif=elapsed.count();

		pid_d= kd*((error-last_error));

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
		


		//Commenting out this line.Uncomment this line to resume previous functioning 
		trq=torque;
		if(st_13+trq < 3888)
		{
		twist.angular.x = st_13+(trq);
		twist.angular.y = st_13-(trq);
		}
		else
		{
			twist.angular.x=3888;
			twist.angular.y=2288;
		}


		errp.data=currentp;
		//str_inf has steering angle in P with time stamp(str_inf.P[0]) which is published on /can_str 
		//The value of the constants Kp, Ki, kd are published on str_inf.K[0],str_inf.K[1],str_inf.k[2] respectively
		//The value of the contribution of pid_p, pid_i and pid_d are published on str_inf.K[3],str_inf.K[4],str_inf.K[5]
		// The steering angle without the time stamps for checking can be found out by subscribing to the topic angle
                str_inf.header.frame_id=std::string("camera");
                str_inf.header.stamp = ros::Time::now();
		str_inf.P[0]=currentp;
		str_inf.K[0]=kp;
		str_inf.K[1]=ki;
		str_inf.K[2]=kd;
		str_inf.K[3]=pid_p;
		str_inf.K[4]=pid_i;
		str_inf.K[5]=pid_d;

		angle.data=currentp;


		//publish data
		vel.publish(twist);
		err.publish(errp);
		can_info.publish(str_inf);
		ang.publish(angle);

		ros::spinOnce();
		loop_rate.sleep();
		++count;
		last_error=error;
	}

	return 0;
}
















  



