#include "ros/ros.h"
#include "turtlesim/Pose.h"
#include "geometry_msgs/Twist.h"
#include "turtlesim/Spawn.h"
#include "turtlesim/TeleportAbsolute.h"
#include <string>


ros::Publisher cmdVelPub;
std::string turtleName = "Turtle_Leonardo";
double turtlesimPoseX, turtlesimPoseY;
float linearSpeed = 1.0, angularSpeed = 15.0, PI = 3.1415926535897, 
	sideLength = 11.088889, diagonalLength = 15.682057;


void turtleCallback(const turtlesim::Pose::ConstPtr& msg);
void spawnLeonardo(ros::NodeHandle nh);
void teleportAbsolute(ros::NodeHandle nh);
void moveInLine(double pathLength);
void rotate(double degree);
void moveInSquare();
void moveInTriangle();

int main (int argc, char **argv) {
	ros::init(argc, argv, "turtlebot_subscriber");
	ros::NodeHandle nh;
	cmdVelPub = nh.advertise<geometry_msgs::Twist>(turtleName + "/cmd_vel", 1);
	ros::Subscriber sub = nh.subscribe(turtleName + "/pose", 1, &turtleCallback);

	spawnLeonardo(nh);

	teleportAbsolute(nh);

	moveInSquare();

	moveInTriangle();

	return 0;
}

void turtleCallback(const turtlesim::Pose::ConstPtr& msg) {
	ROS_INFO("Turtle subscriber@[%f, %f, %f]", msg->x, msg->y, msg->theta);

	turtlesimPoseX = msg->x;
	turtlesimPoseY = msg->y;
}

void spawnLeonardo(ros::NodeHandle nh) {
	ros::ServiceClient client1 = nh.serviceClient<turtlesim::Spawn>("/spawn");

	turtlesim::Spawn srv1;
	srv1.request.x = 5.45;
	srv1.request.y = 5.45;
	srv1.request.theta = 0.0;
	srv1.request.name = turtleName;

	client1.call(srv1);
}

void teleportAbsolute(ros::NodeHandle nh) {
	ros::ServiceClient telAbs = nh.serviceClient<turtlesim::TeleportAbsolute>(turtleName + "/teleport_absolute");

	turtlesim::TeleportAbsolute temp;
	temp.request.x = 0;
	temp.request.y = 0;
	temp.request.theta = 0.0;

	telAbs.call(temp);
}

void moveInLine(double pathLength) {
	geometry_msgs::Twist velMsg;

	velMsg.linear.x = linearSpeed;
	velMsg.linear.y = 0;
	velMsg.linear.z = 0;
	velMsg.angular.x = 0;
	velMsg.angular.y = 0;
	velMsg.angular.z = 0;

	double t0 = ros::Time::now().toSec();
	double currDistance = 0.0;
	ros::Rate loop_rate(10);
	
	while (currDistance <= pathLength)
	{
		cmdVelPub.publish(velMsg);

		double t1 = ros::Time::now().toSec();
		currDistance = linearSpeed * (t1-t0);

		loop_rate.sleep();
		ros::spinOnce();
	}

	velMsg.linear.x = 0;
   	cmdVelPub.publish(velMsg);
}

void rotate(double degree) {
	geometry_msgs::Twist velMsg;

	double angSpeedInRad = angularSpeed*2.0*PI/360.0;

	velMsg.linear.x = 0;
	velMsg.linear.y = 0;
	velMsg.linear.z = 0;
	velMsg.angular.x = 0;
	velMsg.angular.y = 0;
	velMsg.angular.z = angSpeedInRad;

	double t0 = ros::Time::now().toSec();
	double currAngle = 0.0;
	ros::Rate loop_rate(100);
	
	while (currAngle < degree*2.0*PI/360.0)
	{
		cmdVelPub.publish(velMsg);

		double t1 = ros::Time::now().toSec();
		currAngle = angSpeedInRad * (t1-t0);

		loop_rate.sleep();
		ros::spinOnce();
	}

	velMsg.angular.z = 0;
   	cmdVelPub.publish(velMsg);
}

void moveInSquare() {
	do {
		moveInLine(sideLength);
		rotate(90.0);
	}
	while (turtlesimPoseX > 1.0 || turtlesimPoseY > 1.0);
}

void moveInTriangle() {
	moveInLine(sideLength);
	rotate(90.0);
	moveInLine(sideLength);
	rotate(135.0);
	moveInLine(diagonalLength);
}