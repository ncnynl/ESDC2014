#include "Image_processor.h"

Image_processor::Image_processor(uint8_t img_source)
{
	printf("Constructing Image processor\n");
	this->state = IMAGE_PROCESS_INIT;
	strcpy(this->winname,"hello, world");
	this->win_exist = 0;
	this->img_source = img_source;
	if(this->img_source == IMG_SOURCE_WEBCAM)
	{
		this->cap = new cv::VideoCapture(0);
		if(!cap->isOpened())
		{
			printf("Image_processor Error: Cannot open camera\n");
			exit(-1);
		}
	}
}

Image_processor::~Image_processor()
{
	printf("Destructing Image processor\n");
}

IMAGE_PROCESS_STATE Image_processor::get_state()
{
	return this->state;
}

uint8_t Image_processor::capture_image()
{
	if(this->img_source == IMG_SOURCE_CELLPHONE)
	{
		return 1;
	}
	else if(this->img_source == IMG_SOURCE_WEBCAM)
	{
		(*this->cap)>>(this->current_img);
		//Check whether valid frame data is captured
		if(this->current_img.data)
			return 1;
		else
			return -1;
	}
	return 1;
}


uint8_t Image_processor::get_image_and_show()
{
	//get image by path
	cv::Mat image;
	if(this->win_exist)
		cv::destroyWindow(this->winname);
	else
		this->win_exist = 1;
	cv::namedWindow(this->winname,CV_WINDOW_AUTOSIZE);
	cv::imshow(this->winname,image);
	cv::waitKey(0);
	return 1;
}

void Image_processor::test()
{
	//create a window named as this->winname
	cv::namedWindow(this->winname,CV_WINDOW_AUTOSIZE);
	while(true)
	{
		if(!this->capture_image())
		{
			printf("Image_processor Error: cannot capture valid image\n");
			continue;
		}
		//if image is capture, show it to the window
		cv::imshow(this->winname,this->current_img);
		//wait for 50ms to capture the next frame
		cv::waitKey(50);
	}
	return ;
}
