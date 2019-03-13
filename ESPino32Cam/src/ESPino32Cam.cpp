#include "ESPino32Cam.h"


ESPino32Cam::ESPino32Cam()
{
	rgb_x = 0;
	rgb_y = 0;
}

esp_err_t ESPino32Cam::init()
{
	  camera_config_t config;
	  config.ledc_channel = LEDC_CHANNEL_0;
	  config.ledc_timer = LEDC_TIMER_0;
	  config.pin_d0 = Y2_GPIO_NUM;
	  config.pin_d1 = Y3_GPIO_NUM;
	  config.pin_d2 = Y4_GPIO_NUM;
	  config.pin_d3 = Y5_GPIO_NUM;
	  config.pin_d4 = Y6_GPIO_NUM;
	  config.pin_d5 = Y7_GPIO_NUM;
	  config.pin_d6 = Y8_GPIO_NUM;
	  config.pin_d7 = Y9_GPIO_NUM;
	  config.pin_xclk = XCLK_GPIO_NUM;
	  config.pin_pclk = PCLK_GPIO_NUM;
	  config.pin_vsync = VSYNC_GPIO_NUM;
	  config.pin_href = HREF_GPIO_NUM;
	  config.pin_sscb_sda = SIOD_GPIO_NUM;
	  config.pin_sscb_scl = SIOC_GPIO_NUM;
	  config.pin_pwdn = PWDN_GPIO_NUM;
	  config.pin_reset = RESET_GPIO_NUM;
	  config.xclk_freq_hz = 20000000;
	  config.pixel_format = PIXFORMAT_JPEG;
	  //init with high specs to pre-allocate larger buffers
	  if(psramFound()){
		config.frame_size = FRAMESIZE_UXGA;
		config.jpeg_quality = 8;
		config.fb_count = 2;
	  } 
	  else {
		config.frame_size = FRAMESIZE_SVGA;
		config.jpeg_quality = 12;
		config.fb_count = 1;
	  }
		esp_err_t err = esp_camera_init(&config);
	return err; 
}
sensor_t * ESPino32Cam::sensor()
{
	return  esp_camera_sensor_get();
}
camera_fb_t * ESPino32Cam::capture()
{
	return esp_camera_fb_get();
}
void ESPino32Cam:: drawFaceBoxes(dl_matrix3du_t *image_matrix, box_array_t *boxes,bool landmask)
{
    int x, y, w, h, i;
    uint32_t color = FACE_COLOR_YELLOW;
    fb_data_t fb;
    fb.width = image_matrix->w;
    fb.height = image_matrix->h;
    fb.data = image_matrix->item;
    fb.bytes_per_pixel = 3;
    fb.format = FB_BGR888;
    for (i = 0; i < boxes->len; i++){
        // rectangle box
        x = (int)boxes->box[i].box_p[0];
        y = (int)boxes->box[i].box_p[1];
        w = (int)boxes->box[i].box_p[2] - x + 1;
        h = (int)boxes->box[i].box_p[3] - y + 1;
		rbgDrawBox(&fb,color,x,y,w,h);
		if(landmask)
		{
			int x0, y0, j;
			for (j = 0; j < 10; j+=2) {
				x0 = (int)boxes->landmark[i].landmark_p[j];
				y0 = (int)boxes->landmark[i].landmark_p[j+1];
				fb_gfx_fillRect(&fb, x0, y0, 3, 3, color);
			}
		}
	}
}
void ESPino32Cam:: rbgDrawBox(fb_data_t *fb,uint32_t color,int x,int y,int w,int h)
{
	 fb_gfx_drawFastHLine(fb, x, y, w, color);
     fb_gfx_drawFastHLine(fb, x, y+h-1, w, color);
     fb_gfx_drawFastVLine(fb, x, y, h, color);
     fb_gfx_drawFastVLine(fb, x+w-1, y, h, color);
}
size_t ESPino32Cam:: jpg2RGB888(camera_fb_t * fb,dl_matrix3du_t **image_matrix)
{
	*image_matrix = dl_matrix3du_alloc(1,fb->width, fb->height,3);
	if (!*image_matrix)
	{
		return(0);
	}
	bool s = fmt2rgb888(fb->buf, fb->len, fb->format, (*image_matrix)->item);
	if(!s)
	{
		dl_matrix3du_free(*image_matrix);
		Serial.println("to rgb888 failed");
		return(0);
	}
	return(fb->width*fb->height*3);
}
// print text to RGB 

void ESPino32Cam:: rgbGoto(int32_t x, int32_t y)
{
	rgb_x = x;
	rgb_y = y;
}

void ESPino32Cam:: rgbPrint(dl_matrix3du_t *image_matrix, uint32_t color, const char * str){
    fb_data_t fb;
    fb.width = image_matrix->w;
    fb.height = image_matrix->h;
    fb.data = image_matrix->item;
    fb.bytes_per_pixel = 3;
    fb.format = FB_BGR888;
    //fb_gfx_print(&fb, (fb.width - (strlen(str) * 14)) / 2, 10, color, str);
	fb_gfx_print(&fb,rgb_x ,rgb_y,color, str);
}

int ESPino32Cam:: rgbPrintf(dl_matrix3du_t *image_matrix, uint32_t color, const char *format, ...){
    char loc_buf[64];
    char * temp = loc_buf;
    int len;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    len = vsnprintf(loc_buf, sizeof(loc_buf), format, arg);
    va_end(copy);
    if(len >= sizeof(loc_buf)){
        temp = (char*)malloc(len+1);
        if(temp == NULL) {
            return 0;
        }
    }
    vsnprintf(temp, len+1, format, arg);
    va_end(arg);
    rgbPrint(image_matrix, color, temp);
    if(len > 64){
        free(temp);
    }
    return len;
}
