/*
 * Copyright 2023 NXP 
 * SPDX-License-Identifier: BSD-3-Clause
*/

#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/wayland/wayland.h"  
#include "lv_drivers/indev/evdev.h"
#include <unistd.h>
#include <pthread.h>
#include <mqueue.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <signal.h>
#include "lvgl/examples/lv_examples.h"
#include <cstdio>

#define H_RES 1080
#define V_RES 1920

#define DISP_BUF_SIZE H_RES * V_RES * 4 * 2

#define QUEUE_NAME "/voice_queue"
#define MAX_SIZE 1024
#define QUEUE_PERMISSIONS 0666
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256

// Global Functions
void hal_init();

void* sendingThread(void* args);
void* renderingThread(void* args);

// Global variables
lv_obj_t * label1;
lv_obj_t * label2;
lv_obj_t * label3;

pthread_mutex_t mutex;

void sig_handler(int signum)
{
	printf("\nInside Signal handler function\n");
	printf("------SIGINT signal catched------\n");
	printf("Program exit...\n");
    lv_deinit();
    lv_wayland_deinit();
	
	exit(0);
}


bool close_cb(lv_disp_t * disp)
{
	printf("\nInside close callback function!!\n");
	printf("------Close event catched------\n");
	printf("Program exit...\n");
    lv_deinit();
    lv_wayland_deinit();
	
    exit(0);
}

typedef struct{
	bool pass;
	char* plate;
	char* message;	
}user_type;

typedef struct{
    unsigned short id;
    const char* name;
}vit_command_t;


int render_output(char* plate, char* message, bool pass)
{	
    lv_label_set_text_fmt(label2, "%s", plate);
    lv_label_set_text_fmt(label3, "%s", message);

	if(pass) lv_obj_set_style_bg_color(lv_scr_act(), lv_palette_main(LV_PALETTE_GREEN), LV_PART_MAIN);
	else lv_obj_set_style_bg_color(lv_scr_act(), lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);
	lv_obj_invalidate(lv_scr_act());
	sleep(5);

    lv_label_set_text(label1, "");
    lv_label_set_text(label2, "");
    lv_label_set_text(label3, "");
	lv_obj_set_style_bg_color(lv_scr_act(), LV_COLOR_MAKE(0xFF, 0xFF, 0xFF), LV_PART_MAIN);
	lv_obj_invalidate(lv_scr_act());
	return 0;
}


int main(void)
{
	signal(SIGINT, sig_handler); // Register signal handler

    /*LittlevGL init*/
    lv_init();
	hal_init();

    pthread_t thread1;
    pthread_t thread2;

	pthread_mutex_init(&mutex, NULL);
	system("rm /dev/mqueue/voice_queue");

    pthread_create(&thread1, NULL, &sendingThread, NULL);
	printf("Sending Thread Created\r\n");
	pthread_create(&thread2, NULL, &renderingThread, NULL);
	printf("Rendering Thread Created\r\n");

    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) 
    {
        lv_timer_handler();
        usleep(5000);
    }

    return 0;
}

void* sendingThread(void* args)
{
	mqd_t mq;
	struct mq_attr attr;
	int status = 0;

	printf("Task 1: Sending Thread called\n");
	
/*
	user[1]->pass=false;
	user[2]->pass=false;
	user[3]->pass=true;
	user[4]->pass=false;
	user[5]->pass=true;
	user[6]->pass=true;
	user[7]->pass=false;
	user[8]->pass=true;
	user[9]->pass=false;

	user[1]->message="PAGO PENDIENTE";
	user[2]->message="PLACA NO REGISTRADA";
	user[3]->message="BIENVENIDO";
	user[4]->message="PAGO PENDIENTE";
	user[5]->message="BIENVENIDO";
	user[6]->message="BIENVENIDO";
	user[7]->message="PAGO PENDIENTE";
	user[8]->message="BIENVENIDO";
	user[9]->message="PLACA NO REGISTRADA";

	user[1]->plate="GLA-33-89";
	user[2]->plate="TNV-37-56";
	user[3]->plate="JKZ-14-75";
	user[4]->plate="LCR-26-08";
	user[5]->plate="WYP-02-85";
	user[6]->plate="DHE-47-83";
	user[7]->plate="SFB-02-49";
	user[8]->plate="MTR-37-56";
	user[9]->plate="KLZ-17-53";
*/
	attr.mq_flags = 0;
	attr.mq_maxmsg = MAX_MESSAGES;
	attr.mq_msgsize = MAX_MSG_SIZE;
	attr.mq_curmsgs = 0;
	
	mq = mq_open(QUEUE_NAME, O_WRONLY | O_CREAT, QUEUE_PERMISSIONS, &attr);
	if(mq == -1) perror("mq_open failure from Sending Thread");

	printf("Task 1: Message Queue successfully created! \n");
	int index=0;
	while(1){
//    	pthread_mutex_lock(&mutex);
			printf("Task 1: Sending message... \n");
		    mq_send(mq, (const char*) user, sizeof(user_type), 0);
			if (index>=9) index = 0;
			else index++;
			sleep(5);
//    	pthread_mutex_unlock(&mutex);
	} 
}

void* renderingThread(void* args)
{
	mqd_t mq;
    char receiving_buffer[MAX_SIZE];
    int bytes_read=0;
	
	user_type *user;

	printf("Task 2: Rendering Thread called\n");
	mq = mq_open(QUEUE_NAME, O_RDONLY);
	if (mq == -1) perror("mq_open failure from Receiving Thread\n");


	static lv_style_t style;
	lv_style_init(&style);
  
	lv_style_set_text_font(&style, &lv_font_montserrat_48);
	lv_style_set_text_color(&style, lv_color_white());


    printf("Task 2: Drawing white BG...\n");
	lv_obj_set_style_bg_color(lv_scr_act(), LV_COLOR_MAKE(0xFF, 0xFF, 0xFF), LV_PART_MAIN);

    printf("Task 2: Drawing Logo...\n");
    LV_IMG_DECLARE(logo_palomar);
    lv_obj_t * img1 = lv_img_create(lv_scr_act());
    lv_img_set_src(img1, &logo_palomar);
    lv_obj_align(img1, LV_ALIGN_CENTER, -190, -400);
    lv_obj_set_size(img1, 740, 292);
	lv_obj_invalidate(lv_scr_act());
	
	lv_refr_now(NULL);

    printf("Task 2: Drawing Label 1...\n");
    label1 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
    lv_label_set_text(label1, "PLACAS:");
	lv_obj_add_style(label1, &style, 0);
    lv_obj_set_width(label1, 500);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_TOP_MID, -170, 100);

    printf("Task 2: Drawing Label 2...\n");
    label2 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label2, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    lv_label_set_recolor(label2, true);                      /*Enable re-coloring by commands in the text*/
	lv_obj_add_style(label2, &style, 0);
    lv_obj_set_width(label2, 500);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label2, LV_ALIGN_TOP_MID, -170, 200);


    printf("Task 2: Drawing Label 3...\n");
    label3 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label3, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    lv_label_set_recolor(label3, true);                      /*Enable re-coloring by commands in the text*/
	lv_obj_add_style(label3, &style, 0);
    lv_obj_set_width(label3, 600);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label3, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label3, LV_ALIGN_TOP_MID, -170, 800);

    sleep(5);

/*
	char plate[]="WJX-58-73\n";
	char message[]="BIENVENIDO\n";
	bool pass=true;

	char plate2[]="ZWY-08-92\n";
	char message2[]="PLACAS NO REGISTRADAS\n";

	char plate3[]="GHV-14-62\n";
	char message3[]="PAGO PENDIENTE\n";

	render_output(plate, message, pass);
	sleep(3);
	render_output(plate2, message2, false);
	sleep(3);
	render_output(plate3, message3, false);
	sleep(3);
	render_output(plate2, message, true);
	sleep(3);
*/

	while(1){
//    	pthread_mutex_lock(&mutex);
        printf("Task 2: Receiving message...\n");
        bytes_read = mq_receive(mq, receiving_buffer, MAX_SIZE, 0);
		if(bytes_read == -1) perror("Failed to receive message\n");
		if(bytes_read > 0 ){
			user = (user_type*)(receiving_buffer);
			printf("Message received!!\n");
			printf("Message: %s, Plates = %s, Message = %s \n", user->pass ? "true":"false", user->plate, user->message);
			render_output(user->plate, user->message, user->pass);
		}
//    	pthread_mutex_unlock(&mutex);
		sleep(5);
	}
}

void hal_init()
{
	lv_disp_t * disp;

#if 0 /*framebuffer*/
	/*Linux frame buffer device init*/
	fbdev_init();
	
	static lv_color_t buf1[DISP_BUF_SIZE];
	static lv_color_t buf2[DISP_BUF_SIZE];

    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, DISP_BUF_SIZE);

    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = fbdev_flush;
    disp_drv.hor_res    = H_RES;
    disp_drv.ver_res    = V_RES;
    disp = lv_disp_drv_register(&disp_drv);    
#endif
#if 1 /*wayland*/	
    lv_wayland_init();
    disp = lv_wayland_create_window(H_RES, V_RES, "Window Title", close_cb);
#endif

	printf("Disp %d \n", disp);

//	lv_wayland_window_set_fullscreen(disp, true);	
//	lv_obj_set_style(lv_scr_act(), &lv_style_transp);
//	lv_disp_set_bg_opa(NULL, LV_OPA_TRANSP);
//	lv_obj_set_style_local_bg_opa(lv_scr_act(), LV_OBJMASK_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
//	lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_TRANSP, LV_PART_MAIN);
//	lv_img_set_zoom(img1, 10);


}
