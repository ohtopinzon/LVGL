/*
 * Copyright 2023 NXP 
 * SPDX-License-Identifier: BSD-3-Clause
*/

#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lv_drivers/wayland/wayland.h"  
#include "lv_drivers/indev/evdev.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include "src/ui_ext.h"
#include "src/command-interface.h"
#include <stdlib.h>
#include <signal.h>
#include "lvgl/examples/lv_examples.h"
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>

#define H_RES (1280)
#define V_RES (720)

#define DISP_BUF_SIZE (128 * 1024)

void sig_handler(int signum)
{
	printf("\nInside Signal handler function\n");
	printf("------SIGINT signal catched------\n");
	printf("Program exit...\n");
    lv_deinit();
    lv_wayland_deinit();
	
	system("/home/root/.nxp-demo-experience/scripts/multimedia/smart-kitchen/restore.sh");	
	exit(0);
}


bool close_cb(lv_disp_t * disp)
{
	printf("\nInside close callback function!!\n");
	printf("------Close event catched------\n");
	printf("Program exit...\n");
    lv_deinit();
    lv_wayland_deinit();
	
	system("/home/root/.nxp-demo-experience/scripts/multimedia/smart-kitchen/restore.sh");	
    exit(0);
}


/**
 * Open a video from a file
 */
void lv_example_ffmpeg(void)
{
    /*birds.mp4 is downloaded from http://www.videezy.com (Free Stock Footage by Videezy!)
     *https://www.videezy.com/abstract/44864-silhouettes-of-birds-over-the-sunset*/
    lv_obj_t * player = lv_ffmpeg_player_create(lv_scr_act());
    lv_ffmpeg_player_set_src(player, "./lvgl/examples/libs/ffmpeg/birds.mp4");
    lv_ffmpeg_player_set_auto_restart(player, true);
    lv_ffmpeg_player_set_cmd(player, LV_FFMPEG_PLAYER_CMD_START);
    lv_obj_center(player);
}

int main(void)
{
	lv_disp_t * disp;

	signal(SIGINT, sig_handler); // Register signal handler

    /*LittlevGL init*/
    lv_init();
    /*Linux Wayland device init*/
    lv_wayland_init();
    disp = lv_wayland_create_window(H_RES, V_RES, "Window Title", close_cb);

	printf("Disp %d \n", disp);
    //lv_wayland_window_set_fullscreen(disp, true);

    /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf[DISP_BUF_SIZE];

    /* Demo init */
//    ui_ext_init();
	lv_example_ffmpeg();
//	startCommandInterface();

    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) 
    {
        lv_timer_handler();
        usleep(5000);
    }

    return 0;
}

