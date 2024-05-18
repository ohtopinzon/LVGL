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
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <signal.h>
#include "lvgl/examples/lv_examples.h"
#include <cstdio>

#define H_RES 1280
#define V_RES 720

#define DISP_BUF_SIZE H_RES * V_RES * 4

void sig_handler(int signum)
{
	printf("\nInside Signal handler function\n");
	printf("------SIGINT signal catched------\n");
	printf("Program exit...\n");
    lv_deinit();
//    lv_wayland_deinit();
	
	exit(0);
}


bool close_cb(lv_disp_t * disp)
{
	printf("\nInside close callback function!!\n");
	printf("------Close event catched------\n");
	printf("Program exit...\n");
    lv_deinit();
//    lv_wayland_deinit();
	
    exit(0);
}


/**
 * Open a video from a file
 */

int main(void)
{
	lv_disp_t * disp;
	signal(SIGINT, sig_handler); // Register signal handler

    /*LittlevGL init*/
    lv_init();

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

//	lv_obj_set_style(lv_scr_act(), &lv_style_transp);
	lv_disp_set_bg_opa(NULL, LV_OPA_TRANSP);
//	lv_obj_set_style_local_bg_opa(lv_scr_act(), LV_OBJMASK_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
	lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_TRANSP, LV_PART_MAIN);
    /* Demo init */

	//lv_demo_widgets();
    static lv_style_t style_orange;
    lv_style_init(&style_orange);
    lv_style_set_bg_color(&style_orange, lv_color_make(249, 141, 0)); // BLUE RED GREEN 
    lv_style_set_bg_opa(&style_orange, LV_OPA_COVER);


	lv_obj_t * btn1 = lv_btn_create(lv_scr_act());                   /*Add a button to the current screen*/
	lv_obj_set_pos(btn1, 10, V_RES - 60);                                    /*Set its position*/
	lv_obj_set_size(btn1, 100, 50);                                  /*Set its size*/

	lv_obj_t * label = lv_label_create(btn1);                        /*Add a label to the button*/
	lv_label_set_text(label, "Button 1");                             /*Set the labels text*/
	lv_obj_center(label);                                           /*Align the label to the center*/

	lv_obj_t * btn2 = lv_btn_create(lv_scr_act());                   /*Add a button to the current screen*/
	lv_obj_set_pos(btn2, H_RES - 110, V_RES - 60);                                    /*Set its position*/
	lv_obj_set_size(btn2, 100, 50);                                  /*Set its size*/
	
	lv_obj_t * label2 = lv_label_create(btn2);                        /*Add a label to the button*/
	lv_label_set_text(label2, "Button 2");                             /*Set the labels text*/
	lv_obj_center(label2);                                           /*Align the label to the center*/

    lv_obj_add_style(btn1, &style_orange, 0);
    lv_obj_add_style(btn2, &style_orange, 0);
    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) 
    {
        lv_timer_handler();
        usleep(5000);
    }

    return 0;
}

