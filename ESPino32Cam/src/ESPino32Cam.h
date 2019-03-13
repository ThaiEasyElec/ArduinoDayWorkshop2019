#ifndef _ESPino32Cam_H_
#define _ESPino32Cam_H_
#include "Arduino.h"

#include "esp_camera.h"
#include "fb_gfx.h"
#include "fd_forward.h"
#include "dl_lib.h"
#include "fr_forward.h"

#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM     4
#define XCLK_GPIO_NUM      13
#define SIOD_GPIO_NUM     21
#define SIOC_GPIO_NUM     22

#define Y9_GPIO_NUM       34
#define Y8_GPIO_NUM       35
#define Y7_GPIO_NUM       32
#define Y6_GPIO_NUM       25
#define Y5_GPIO_NUM       27
#define Y4_GPIO_NUM       12
#define Y3_GPIO_NUM       14
#define Y2_GPIO_NUM       26
#define VSYNC_GPIO_NUM    36
#define HREF_GPIO_NUM     39
#define PCLK_GPIO_NUM     33

#define FACE_COLOR_WHITE  0x00FFFFFF
#define FACE_COLOR_BLACK  0x00000000
#define FACE_COLOR_RED    0x000000FF
#define FACE_COLOR_GREEN  0x0000FF00
#define FACE_COLOR_BLUE   0x00FF0000
#define FACE_COLOR_YELLOW (FACE_COLOR_RED | FACE_COLOR_GREEN)
#define FACE_COLOR_CYAN   (FACE_COLOR_BLUE | FACE_COLOR_GREEN)
#define FACE_COLOR_PURPLE (FACE_COLOR_BLUE | FACE_COLOR_RED)

#define ENROLL_CONFIRM_TIMES 5
#define FACE_ID_SAVE_NUMBER 7

class ESPino32Cam
{
	public:
	
	ESPino32Cam();
	esp_err_t init();
	sensor_t * sensor();
	camera_fb_t * capture();
	size_t jpg2RGB888(camera_fb_t * fb,dl_matrix3du_t **image_matrix);
	void rbgDrawBox(fb_data_t *fb,uint32_t color,int x,int y,int w,int h);
	void drawFaceBoxes(dl_matrix3du_t *image_matrix, box_array_t *boxes,bool landmask=false);
	void rgbGoto(int32_t x, int32_t y);
	void rgbPrint(dl_matrix3du_t *image_matrix, uint32_t color, const char * str);
	int  rgbPrintf(dl_matrix3du_t *image_matrix, uint32_t color, const char *format, ...);

	private:
	 int rgb_x;
	 int rgb_y;
	 
};











#endif