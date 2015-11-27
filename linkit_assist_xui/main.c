/*
 This sample code is in public domain.

 This sample code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/* 
 This sample shows a line that keeps spinning on top of a gradient
 background image.

 It decodes a PNG image resource into a frame buffer with
 vm_graphic_draw_image_memory,
 and draws a line with vm_graphic_draw_line, then start a timer with
 vm_timer_create_precise for animation.
 The timer runs every 1/10 secs, resulting in a 10 FPS animation. For each
 frame in the animation, vm_graphic_linear_transform is used to rotate
 the line, and then vm_graphic_blt_frame is called to composite the rotated line
 with the background image, and displays the result.

 Modify the macro SCREEN_WIDTH and SCREEN_HEIGHT to fit your display device.
 */

#include "vmtype.h"
#include "vmsystem.h"
#include "vmres.h"
#include "vmlog.h"
#include "ResID.h"
#include "vmtimer.h"
#include "vmgraphic.h"
#include "vmthread.h"
#include "xui.h"

/* Animation timer */
VM_TIMER_ID_PRECISE g_timer_id;

xui_page page;
xui_text_view text_view;
void * views[] = { &text_view };
VMINT x = 0, y = 0;
VMBOOL visibility = VM_TRUE;

/* Update the rotating line, then update the display */
static void timer_callback(VM_TIMER_ID_PRECISE tid, void* user_data) {
	vm_log_debug("timer_callback");
	xui_view_set_visibility(text_view.view, visibility);
	visibility = !visibility;
	xui_view_set_x(text_view.view, ++x);
	xui_view_set_y(text_view.view, ++y);
	vm_graphic_color_argb_t color;
	color.a = 128 - x; // alpha value changes
	color.r = 255;
	color.g = 0;
	color.b = 0;
	xui_text_view_set_text_color(text_view.view, color);
	xui_validate(page);
}

static vm_graphic_color_argb_t white() {
	vm_graphic_color_argb_t color;
	color.a = 255;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	return color;
}

/* The callback to be invoked by the system engine. */
void handle_system_event(VMINT message, VMINT param) {
	switch (message) {
	case VM_EVENT_CREATE:
		xui_init();
		text_view = xui_init_text_view();
		page = xui_init_page((void**) &views, sizeof(views) / sizeof(void *));
		xui_page_set_background_color(page.page, white());
		break;
	case VM_EVENT_PAINT:
		/* Graphics library is ready to use, start drawing */
		// vm_thread_sleep(10 * 1000);
		g_timer_id = vm_timer_create_precise(1000, timer_callback, NULL);
		timer_callback(g_timer_id, NULL);
		break;
	case VM_EVENT_QUIT:
		xui_release();
		vm_timer_delete_precise(g_timer_id);
		/* Deinit APP resource */
		//vm_res_release();
		break;
	}
}

/* Entry point */
void vm_main(void) {
	xui_lcd_st7789s_init();
	xui_lcd_backlight_level(60);
	/* register system events handler */
	vm_pmng_register_system_event_callback(handle_system_event);
}
