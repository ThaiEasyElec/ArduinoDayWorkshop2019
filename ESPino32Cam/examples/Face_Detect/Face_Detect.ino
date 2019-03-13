#include "ESPino32Cam.h"
ESPino32Cam cam;
static mtmn_config_t mtmn_config = {0};

void setup() {
  Serial.begin(1000000);
  Serial.println(F("Initial Camera :"));
  if (cam.init() != ESP_OK)
  {
    Serial.println(F("Fail"));
    while (1);
  }
  Serial.println(F("OK"));
  //Camera Setting
  sensor_t *s = cam.sensor();
  s->set_framesize(s, FRAMESIZE_QVGA);
  s->set_whitebal(s, true);
  //MTMN Config
  mtmn_config.min_face = 160;
  mtmn_config.pyramid = 0.7;
  mtmn_config.p_threshold.score = 0.6;
  mtmn_config.p_threshold.nms = 0.7;
  mtmn_config.r_threshold.score = 0.7;
  mtmn_config.r_threshold.nms = 0.7;
  mtmn_config.r_threshold.candidate_number = 4;
  mtmn_config.o_threshold.score = 0.7;
  mtmn_config.o_threshold.nms = 0.4;
  mtmn_config.o_threshold.candidate_number = 1;
}

void loop()
{
  capture();
}

esp_err_t capture()
{
  /*  Capture */
  camera_fb_t * fb = NULL;
  fb = cam.capture();
  if (!fb)
  {
    Serial.println("Camera capture failed");
    return ESP_FAIL;
  }

  /*  Convert JPG to RGB888 */
  dl_matrix3du_t *image_matrix;
  size_t out_width = fb->width;
  size_t out_height = fb->height;
  size_t out_len = cam.jpg2RGB888(fb, &image_matrix);
  if (out_len == 0)
  {
    esp_camera_fb_return(fb);
    return ESP_FAIL;
  }
  esp_camera_fb_return(fb);

  /*  Face Detect */
  box_array_t *net_boxes = face_detect(image_matrix, &mtmn_config); // Face Detect
  if (net_boxes)
  {
    drawFaceBox(image_matrix, net_boxes, false); // Draw Bolder Box
    free(net_boxes->box);
    free(net_boxes->landmark);
    free(net_boxes);
  }
  /*  Convert RGB888 to JPG */
  uint8_t *jpgout;
  size_t  jpglen;
  bool s = fmt2jpg(image_matrix->item, out_len, out_width, out_height, PIXFORMAT_RGB888, 90, &jpgout, &jpglen); //Convert RGB888 To JPG
  if (s)
  {
    Serial.write(jpgout, jpglen); // Send jpg to SerialPort
  }
  /* Free memory*/
  free(jpgout);
  dl_matrix3du_free(image_matrix);
}

void drawFaceBox(dl_matrix3du_t *image_matrix, box_array_t *boxes, bool landmask)
{
  int x, y, w, h, i;
  uint32_t color = FACE_COLOR_YELLOW;
  fb_data_t fb;
  fb.width = image_matrix->w;
  fb.height = image_matrix->h;
  fb.data = image_matrix->item;
  fb.bytes_per_pixel = 3;
  fb.format = FB_BGR888;
  for (i = 0; i < boxes->len; i++)
  {
    // rectangle box
    x = (int)boxes->box[i].box_p[0];
    y = (int)boxes->box[i].box_p[1];
    w = (int)boxes->box[i].box_p[2] - x + 1;
    h = (int)boxes->box[i].box_p[3] - y + 1;
    cam.rbgDrawBox(&fb, color, x, y, w, h);
    if (landmask)
    {
      int x0, y0, j;
      for (j = 0; j < 10; j += 2) {
        x0 = (int)boxes->landmark[i].landmark_p[j];
        y0 = (int)boxes->landmark[i].landmark_p[j + 1];
        fb_gfx_fillRect(&fb, x0, y0, 3, 3, color);
      }
    }
  }
}
