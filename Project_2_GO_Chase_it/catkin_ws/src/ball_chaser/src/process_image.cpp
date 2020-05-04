#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // DONE: Request a service and pass the velocities to it to drive the robot
	ball_chaser::DriveToTarget srv;
	srv.request.linear_x = lin_x;
	srv.request.angular_z = ang_z;
	// Call the command_robot service
	if (!client.call(srv))
	        ROS_ERROR("Failed to call service command_robot");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    bool is_white = false;
    int row_found;
    float lin_x = 0;
    float ang_z = 0;
    // DONE: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
	for(int i = 0; i < img.height * img.step; i+=3)
	{
	   if(img.data[i] == white_pixel && img.data[i+1] == white_pixel && img.data[i+2] == white_pixel)
	   {
			is_white = true;
	   		row_found = i % img.step;
		   if(row_found < (img.step/3))
		   {
				//turn left
				lin_x = 0.2;
				ang_z = 0.4;
		   }
		   else if(row_found > (img.step * (2.0/3)))
		   {
			   //turn right
			   lin_x = 0.2;
			   ang_z = -0.4; 
		   } 
		   else{
			//Move FORWARD
			lin_x = 0.5;
			ang_z = 0;           
			}
			break;            
			}
	}
	drive_robot(lin_x, ang_z);

}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
