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

#define H_RES 1080
#define V_RES 1920

#define DISP_BUF_SIZE H_RES * V_RES * 4 * 2

//Global variables
lv_obj_t * label1;
lv_obj_t * label2;
lv_obj_t * label3;


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

typedef struct{
	char *plate;
	char *message;	
	bool pass;
}user_type;

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
//	lv_disp_set_bg_opa(NULL, LV_OPA_TRANSP);
//	lv_obj_set_style_local_bg_opa(lv_scr_act(), LV_OBJMASK_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
//	lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_TRANSP, LV_PART_MAIN);


	static lv_style_t style;

	lv_style_init(&style);
  
	lv_style_set_text_font(&style, &lv_font_montserrat_48);
	lv_style_set_text_color(&style, lv_color_white());


    label1 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
    lv_label_set_text(label1, "PLACAS:");
	lv_obj_add_style(label1, &style, 0);
    lv_obj_set_width(label1, 500);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_TOP_MID, -170, 100);

    label2 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label2, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    lv_label_set_recolor(label2, true);                      /*Enable re-coloring by commands in the text*/
	lv_obj_add_style(label2, &style, 0);
    lv_obj_set_width(label2, 500);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label2, LV_ALIGN_TOP_MID, -170, 200);


    label3 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label3, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    lv_label_set_recolor(label3, true);                      /*Enable re-coloring by commands in the text*/
	lv_obj_add_style(label3, &style, 0);
    lv_obj_set_width(label3, 600);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label3, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label3, LV_ALIGN_TOP_MID, -170, 800);

    LV_IMG_DECLARE(logo_palomar);
    lv_obj_t * img1 = lv_img_create(lv_scr_act());
    lv_img_set_src(img1, &logo_palomar);
    lv_obj_align(img1, LV_ALIGN_CENTER, -190, -400);
    lv_obj_set_size(img1, 740, 292);

	lv_obj_set_style_bg_color(lv_scr_act(), LV_COLOR_MAKE(0xFF, 0xFF, 0xFF), LV_PART_MAIN);

    sleep(5);

	char plate[]="AAAAAA\n";
	char message[]="Bienvenido\n";
	bool pass=true;

	render_output(plate, message, pass);
//	lv_img_set_zoom(img1, 10);

    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) 
    {
        lv_timer_handler();
        usleep(5000);
    }

    return 0;
}

