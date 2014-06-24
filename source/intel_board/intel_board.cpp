/*
 * Copyright (C) Your copyright notice.
 *
 * Author: Edward HUANG@CUHK huangxx_2155@live.com
 *         Tony Yi@CUHK     
 *         Terry Lai@CUHK
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the PG_ORGANIZATION nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY	THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS-IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "intel_board.h"

intel_board::intel_board(uint8_t mode,uint8_t img_source)
{
	//STATE
	//STATE
	//STATE
	switch(mode)
	{
	case(0):
			printf("RUNNING in AUTO MODE\n");
			this->mode = AUTO_MODE;
			break;
	case(1):
			printf("RUNNING in MANUAL MODE\n");
			this->mode = MANUAL_MODE;
			break;
	case(2):
			printf("RUNNING IN IMG_ANALYSIS_MODE\n");
			this->mode = IMG_ANALYSIS_MODE;
			break;
	default:
			printf("Invalid Mode\nExiting program\n");
			exit(1);
	}
	this->state = ROBOT_INIT;
	this->photo_mode = SINGLE_PHOTO;
	this->waist_shot = 0;
	//SUBMODULE
	//SUBMODULE
	//SUBMODULE

	//img_source 1 for WEBCAM MODE, 0 for cellphone mode
	printf("Creating Image_processor()\n");
	this->image_processor = new Image_processor(img_source);

	printf("Creating Motion_controller()\n");
	this->motion_controller = new Motion_controller();
	this->motion_controller->waist_shot = &this->waist_shot;
	printf("Creating UI()\n");
	this->ui = new UI();

	printf("hello intel board\n\n");	
}

intel_board::~intel_board()
{
	printf("~intel_board(): deleting Image_processor\n");
	delete this->image_processor;

	printf("~intel_board(): deleting Motion_controller\n");
	delete this->motion_controller;
	
	printf("~intel_board(): deleting UI\n");
	delete this->ui;
	
	printf("Bye bye intel board\n");
}

uint8_t intel_board::main_function()
{
	printf("Intel board is going to execute its main functionality\n");
	char key;
	if(this->mode == IMG_ANALYSIS_MODE)
	{
		this->robot_only_image_analysis();
	}
	else
	{
		// the main state machine
		while(1)
		{
			switch(this->state)
			{
				case ROBOT_INIT:
					if(this->robot_init())
					{
						this->state = ROBOT_READY;
					}
					else
					{	
						printf("intel_board robot_init() return 0\n");
						exit(-1);
					}
				break;
				
				case ROBOT_READY:
					//the robot will be waiting for user's specific command to continue
					this->robot_ready();
				break;
				
				case ROBOT_FIND_TARGET:
					//working on finding the target
					if(this->robot_find_target())
					{
						this->state = ROBOT_EVALUATE_IMAGE;
						//get the distance according to the face detection result (running four point algorithm)
						this->distance = this->image_processor->get_distance(this->image_processor->final_face_detect[0]);
					}
				break;

				case ROBOT_EVALUATE_IMAGE:
					if(this->robot_evaluate_image())
						//if the image is good enough
						//store the image and go back wait for the next command
						this->state = ROBOT_WAIT_FOR_ADJUSTMENT;
					else
						//let the system analyze the image and find out possible method to make it better
						this->state = ROBOT_ANALYZE_IMAGE;
				break;
				
				case ROBOT_ANALYZE_IMAGE:
					this->robot_analyze_image();
					this->state = ROBOT_APPROACH_REF;
					break;
				
				case ROBOT_APPROACH_REF:
					this->robot_approach_ref();
					//take another picture and check whether the target is in scope
					printf("intel_board: ROBOT_APPROACH_REF finished\n");
					uint8_t flags;
					if((*this->motion_controller->waist_shot))
						flags = ENABLE_FACE_DETECT;
					else
						flags = ENABLE_FACE_DETECT | ENABLE_BODY_DETECT;

					while(this->image_processor->one_target_in_scope(flags) == 0)
					{
						if(this->waist_shot)
							this->image_processor->mark_exp_region(this->motion_controller->face_ref);
						else		
							this->image_processor->mark_exp_region(this->motion_controller->ref);
						this->image_processor->show_analyzed_img();
					}
					
					if(this->waist_shot)
						this->image_processor->mark_exp_region(this->motion_controller->face_ref);
					else		
						this->image_processor->mark_exp_region(this->motion_controller->ref);
					this->image_processor->show_analyzed_img();
					printf("Intel_board: GOING TO EVALUATE THE NEW IMAGE\n");
					printf("Intel_board: GOING TO EVALUATE THE NEW IMAGE\n");
					printf("Intel_board: GOING TO EVALUATE THE NEW IMAGE\n");
					printf("Intel_board: GOING TO EVALUATE THE NEW IMAGE\n");
					printf("Intel_board: GOING TO EVALUATE THE NEW IMAGE\n");
					printf("Intel_board: GOING TO EVALUATE THE NEW IMAGE\n");
					//go back for evaulat image
					this->state = ROBOT_EVALUATE_IMAGE;
					break;
				case ROBOT_WAIT_FOR_ADJUSTMENT:
					this->robot_wait_for_adjustment();
					this->state = ROBOT_READY;
					break;
			}
		}
	}
	return 1;
}

uint8_t intel_board::robot_init()
{
	printf("intel_board: the robot is in init state\n");
	printf("intel_board: setting the task_counter to 0\n");
	this->task_counter = 0;
	if(!this->image_processor->init())
		return 0;
	if(!this->motion_controller->init())
		return 0;
	printf("intel_board: initilization done\n");
	return 1;
}

uint8_t intel_board::robot_ready()
{
	printf("intel_board: the robot is in ready state\n");
	//fetch degree
	int32_t phone_degree = ui->update_degree();
	Message msg;
	msg.CompassRequest();
	printf("intel_board::robot_ready() sending compass request\n");
	msg.sendMessage(this->motion_controller->Com->fd);
	
	int32_t degree,dir;
	degree_rotation(msg.car_degree,phone_degree,&degree,&dir);

	printf("intel_board::robot_ready() get degree from phone %d\n",phone_degree);
	printf("intel_board::robot_ready() get degree from compass %d\n",msg.car_degree);
	printf("intel_board::robot_ready() dir is %d, degree is %d\n",dir,degree);


	if(dir > 0)
		this->motion_controller->rotate((uint16_t) degree,(uint8_t) CAR_ROTATE_RIGHT);
	else
		this->motion_controller->rotate((uint16_t) degree,(uint8_t) CAR_ROTATE_LEFT);
	
	this->waist_shot = 1;

	command_type cmd;

	while(cmd = ui->wait_command())
	{
		if(cmd == start_movement)
		{
			this->task_counter++;
			printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
			printf("INTEL BOARD STARTING A NEW TASK\n!!");
			printf("intel_board: the robot is going to find target\n");
			this->state = ROBOT_FIND_TARGET;
			return 1;
		}
		
		switch(cmd)
		{
			case(pattern_1):
				this->waist_shot = 0;
				this->motion_controller->set_pattern(1);
			break;

			case(pattern_2):
				this->waist_shot = 0;
				this->motion_controller->set_pattern(2);
			break;

			case(pattern_3):
				this->waist_shot = 1;
				this->motion_controller->set_pattern(3);
			break;
			
			case(pattern_4):
				this->waist_shot = 1;
				this->motion_controller->set_pattern(4);
			break;
			
			case(set_waist_shot):
				this->waist_shot = this->waist_shot ? 0:1;
			break;

			default:
				printf("intel_board::robot_ready() undefined reaction for command %d\n",cmd);
			break;
		}
	}
	return 0;
}

uint8_t intel_board::robot_find_target()
{
	printf("intel_board::robot_find_target() running\n");
	uint8_t state = 0;
	uint8_t counter = 0;
	uint16_t dis = 350;
	uint16_t degree = 30;
	uint8_t sec = 0;

	while(!this->image_processor->one_target_in_scope(ENABLE_BODY_DETECT | ENABLE_FACE_DETECT))
	{
		//this->robot_find_target_strategy1(state,counter);
		this->robot_find_target_strategy2(state);
		this->robot_countdown(sec);
	}

	if(this->waist_shot)
		this->image_processor->mark_exp_region(this->motion_controller->face_ref);
	else
		this->image_processor->mark_exp_region(this->motion_controller->ref);

	this->image_processor->show_analyzed_img();
	printf("intel_board::robot_find_target(): TARGET FOUND!\n");
	return 1;
}

uint8_t intel_board::robot_find_target_strategy1(uint8_t &state,uint8_t &counter)
{
	if(this->waist_shot)
		this->image_processor->mark_exp_region(this->motion_controller->face_ref);
	else
		this->image_processor->mark_exp_region(this->motion_controller->ref);
	this->image_processor->show_analyzed_img();
	
	//TODO: may adjust the position according to the initial detection results
	printf("intel_board::robot_find_target(): finding target again\n");
	
	switch(state)
	{
		case 0://repeat state
			if(counter < 2)
			//do nothing
				counter++;
			else
				state = 1;
		break;
		
		case 1://forward [dis] mm
			this->motion_controller->move(DEFAULT_DIS_SMALL,0);
			state = 2;
		break;
		
		case 2://backward [dis] mm
			this->motion_controller->move(DEFAULT_DIS_SMALL * 2,1);
			state = 3;
		break;
		
		case 3: //right side [dis] mm
			this->motion_controller->move(DEFAULT_DIS_SMALL,0);
			this->motion_controller->move(DEFAULT_DIS_SMALL,2);
			state = 4;
		break;
		
		case 4: //left side [dis * 2] mm
			this->motion_controller->move(DEFAULT_DIS_SMALL * 2,3);
			state = 5;
		break;
		
		case 5:
			//move right [dis] mm
			this->motion_controller->move(DEFAULT_DIS_SMALL,2);
			//rotate 30 degree
			this->motion_controller->rotate(DEFAULT_DEGREE,CAR_ROTATE_RIGHT);
			state = 0;
			counter = 0;
		break;
		
		default:
		break;
	}
	return 1;
}

uint8_t intel_board::robot_find_target_strategy2(uint8_t &state)
{
	if(state < 2)
	{
		state++;
	}
	else
	{
		this->motion_controller->rotate(DEFAULT_DEGREE,CAR_ROTATE_RIGHT);
		state = 0;
	}
	return 0;
}
uint8_t intel_board::robot_evaluate_image()
{
	printf("intel_board: robot_evaluate_image() running\n");
	if(this->image_processor->final_face_detect.size() != 0)
		return this->motion_controller->evaluate_image(
			this->image_processor->get_detection_result(),
			this->image_processor->get_face_detection_result(),
			this->distance);
	else
		printf("intel_board::robot_evaluate_image() error: evaluating an image without detection result!\n");
		return 0;
}

uint8_t intel_board::robot_analyze_image()
{
	printf("intel_board::robot_analyze_image() running\n");
	//analyze the image and get the expected movement to take
	//currently it is done in robot_evaluate_image()
	printf("intel_board::robot_analyze_image() exiting\n");
	return 1;
}

uint8_t intel_board::robot_approach_ref()
{
	printf("intel_board::robot_approach_ref() running\n");
	//have the motion_controller send the command to the car
	printf("intel_board::robot_approach_ref() send cmd finished\n");
	return 1;
}

uint8_t intel_board::robot_wait_for_adjustment()
{

	printf("intel_board::robot_wait_for_adjustment() running\n");
	//TODO:
	//should be waiting for adjustment here.

	this->image_processor->cam->save_photo_af();
	this->image_processor->one_target_in_scope(ENABLE_FACE_DETECT);
	if(this->waist_shot)
		this->image_processor->mark_exp_region(this->motion_controller->face_ref);
	else		
		this->image_processor->mark_exp_region(this->motion_controller->ref);
	this->image_processor->show_analyzed_img();
	this->image_processor->save_current_image(this->task_counter);	
	
	this->ui->send_finished_ack();
	command_type cmd;
	while((cmd = ui->wait_command()) != confirm_picture)
	{
		this->robot_act_by_cmd(cmd);
	}

	this->motion_controller->set_lifter(LIFTER_INIT_POS);
	printf("intel_board:: task %d finished\n\n\n",this->task_counter);
	return 1;
}

uint8_t intel_board::robot_only_image_analysis()
{
	DIR *dir;
	struct dirent *ent;
	if((dir = opendir(dir_path)) == NULL)
	{
		perror("");
		return EXIT_FAILURE;
	}
	while((ent = readdir(dir)) != NULL)
	{
		this->image_processor->init();
		if(strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0)
			continue;
		char filename[64];
		strcpy(filename,dir_path);
		strcat(filename,"/");
		strcat(filename,ent->d_name);
		printf("Openning %s\n",filename);
		Image_processor *ptr = this->image_processor;
		ptr->read_image(filename);
		ptr->analyzed_img = ptr->current_img.clone();

		//run the basic body detection algorithm
		//ptr->run_body_detection(ptr->current_img,ptr->body_detect);
		//run the basic face detection algorithm
		ptr->run_body_detection(ptr->current_img,ptr->body_detect);
		ptr->run_face_detection(ptr->current_img,ptr->face_detect);
		//mark the face and body after basic detection algorithm
		//ptr->analyzed_img = ptr->mark_detected_body(ptr->current_img,ptr->body_detect);
		//ptr->analyzed_img = ptr->mark_detected_face(ptr->analyzed_img,ptr->face_detect);
		//show the analyzed img after basic detection algorithm
		//ptr->show_analyzed_img();
		//run a basic filter 
		ptr->basic_filter();
		//mark the face and body after basic filter
		ptr->analyzed_img = ptr->mark_detected_body(ptr->current_img,ptr->final_body_detect);
		ptr->analyzed_img = ptr->mark_detected_face(ptr->analyzed_img,ptr->final_face_detect);
		//show the analyzed img after basic filter
		this->robot_evaluate_image();
		ptr->show_analyzed_img();
	}
}

void intel_board::robot_countdown(uint8_t sec)
{
	useconds_t usec = 1000000;
	printf("intel_board::robot_countdown counting down\n");
	for(uint8_t count = sec;count > 0;count--)
	{
		printf("intel_board::robot_countdown counting down %d\n",count);
		usleep(usec);
	}
	printf("intel_board::robot_countdown finished\n");
}

void intel_board::robot_act_by_cmd(const command_type & cmd)
{
	switch(cmd)
	{
		case(car_forward):
			this->motion_controller->move(DEFAULT_DIS_SMALL,CAR_FORWARD);
		break;
		
		case(car_backward):
			this->motion_controller->move(DEFAULT_DIS_SMALL,CAR_BACKWARD);
		break;
		
		case(car_left):
			this->motion_controller->move(DEFAULT_DIS_SMALL,CAR_LEFT);
		break;

		case(car_right):
			this->motion_controller->move(DEFAULT_DIS_SMALL,CAR_RIGHT);
		break;

		case(lift_up):
			this->motion_controller->lift(LIFTER_SEG,LIFTER_UP);
		break;

		case(lift_down):
			this->motion_controller->lift(LIFTER_SEG,LIFTER_DOWN);
		break;

		default:
			printf("intel_board::robot_act_by_cmd has not yet implemented reaction to %d\n",cmd);
		break;	
	}
}


void degree_rotation(int32_t car,int32_t phone,int32_t *degree,int32_t *direction)
{
	Direction phone_angle=null;
	Direction car_angle=null;

	int32_t rotate_degree=0;//postive means clockwise

	if(phone<=90)
		phone_angle=LOWER_LEFT;
	else if(phone<=180)
		phone_angle=UPPER_LEFT;
	else if(phone<=270)
		phone_angle=UPPER_RIGHT;
	else 
		phone_angle=LOWER_RIGHT;


	if(car<=90)
		car_angle=LOWER_LEFT;
	else if(car<=180)
		car_angle=UPPER_LEFT;
	else if(car<=270)
		car_angle=UPPER_RIGHT;
	else 
		car_angle=LOWER_RIGHT;


		switch(car_angle)
		{
			case UPPER_LEFT:
			if(phone_angle==LOWER_RIGHT)
			{
				int32_t temp_car=180-car;
				int32_t temp_phone=360-phone;
				rotate_degree=temp_car-temp_phone;
				break;
			}
			else if(phone_angle==LOWER_LEFT)
			{

				rotate_degree=(180-car)+phone;
				break;
			}

			else if(phone_angle==UPPER_LEFT)
			{

				rotate_degree=360-((180-phone)+car);
				break;
			}
			else
			{

				rotate_degree=360-(car-(phone-180));
				break;
			}

			case UPPER_RIGHT:

			if(phone_angle==LOWER_LEFT)
			{
				int32_t temp_car=car-180;
				int32_t temp_phone=phone;
				rotate_degree=temp_phone-temp_car;
				break;
			}
			else if(phone_angle==UPPER_LEFT)
			{

				int32_t temp_car=270-car;
				int32_t temp_phone=phone-90;
				rotate_degree=temp_phone+temp_car;
				break;
			}
			else if(phone_angle==UPPER_RIGHT)
			{

				int32_t temp_car=360-car;
				int32_t temp_phone=phone;
				rotate_degree=temp_phone+temp_car;
				break;
			}
			else
			{

				int32_t temp_car=car-180;
				int32_t temp_phone=360-phone;
				rotate_degree=360-(temp_phone+temp_car);
				break;
			}

			case LOWER_LEFT:
			if(phone_angle==UPPER_RIGHT)
			{
				int32_t temp_car=car;
				int32_t temp_phone=phone-180;
				rotate_degree=temp_phone-temp_car;
				break;
			}
			else if(phone_angle==LOWER_RIGHT)
			{
				int32_t temp_car=car;
				int32_t temp_phone=180-(360-phone);
				rotate_degree=temp_phone-temp_car;
				break;
			}
			else if(phone_angle==LOWER_LEFT)
			{
				int32_t temp_car=car;
				int32_t temp_phone=180+phone;
				rotate_degree=temp_phone-temp_car;
				break;
			}

			else 
			{
				int32_t temp_car=car;
				int32_t temp_phone=180-phone;
				rotate_degree=360-(temp_phone+temp_car);
				break;
			}

			case LOWER_RIGHT:
			if(phone_angle==UPPER_LEFT)
			{
				int32_t temp_car=360-car;
				int32_t temp_phone=180-phone;
				rotate_degree=temp_car-temp_phone;
				break;
			}
			else if(phone_angle==UPPER_RIGHT)
			{
				int32_t temp_car=360-car;
				int32_t temp_phone=phone-180;
				rotate_degree=temp_car+temp_phone;
				break;
			}
			else if(phone_angle==LOWER_RIGHT)
			{
				int32_t temp_car=360-car;
				int32_t temp_phone=180-(360-phone);
				rotate_degree=temp_car+temp_phone;
				break;
			}
			else
			{
				int32_t temp_car=360-car;
				int32_t temp_phone=phone+180;
				rotate_degree=temp_car+temp_phone;
				break;
			}


		}//end switch


	if((rotate_degree>=0)&&(rotate_degree<=180))
 	{
 			*degree=rotate_degree;
 			*direction=1;
 	}
 	else if((rotate_degree>=180)&&(rotate_degree<=360))
 	{
 		*degree=360-rotate_degree;
 		*direction=-1;
 	}
 	else if((rotate_degree>=-360)&&(rotate_degree<=-180))
 	{
 		*degree=360-abs(rotate_degree);
 		*direction=1;
 	}
 	else if((rotate_degree>=-180)&&(rotate_degree<=0))
 	{
 		*degree=abs(rotate_degree);
 		*direction=-1;
 	}

}//end void
