# 1 "C:\\Users\\FA19-B~1.CUI\\AppData\\Local\\Temp\\tmpc5vlcyn7"
#include <Arduino.h>
# 1 "C:/Users/FA19-BEE-083.cui/Documents/PlatformIO/Projects/esp32cam/src/ESP32_CAM_WEBSOCKET_CLIENT.ino"
#include "esp_camera.h"
#include <WiFi.h>
#include <ArduinoWebsockets.h>
# 14 "C:/Users/FA19-BEE-083.cui/Documents/PlatformIO/Projects/esp32cam/src/ESP32_CAM_WEBSOCKET_CLIENT.ino"
#define CAMERA_MODEL_AI_THINKER 

#include "camera_pins.h"

const char* ssid = "ibrar";
const char* password = "ibrarahmad";

const char* websockets_server_host = "192.168.4.1";
const uint16_t websockets_server_port = 8888;

using namespace websockets;
WebsocketsClient client;
void setup();
void loop();
#line 27 "C:/Users/FA19-BEE-083.cui/Documents/PlatformIO/Projects/esp32cam/src/ESP32_CAM_WEBSOCKET_CLIENT.ino"
void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

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
  config.xclk_freq_hz = 10000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if(psramFound()){
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }



  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }


  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");

  while(!client.connect(websockets_server_host, websockets_server_port, "/")){
    delay(500);
    Serial.print(".");
  }

  Serial.println("Socket Connected!");
}

void loop() {
  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;
  fb = esp_camera_fb_get();
  if(!fb){
    Serial.println("Camera capture failed");
    esp_camera_fb_return(fb);
    return;
  }

  size_t fb_len = 0;
  if(fb->format != PIXFORMAT_JPEG){
    Serial.println("Non-JPEG data not implemented");
    return;
  }

  client.sendBinary((const char*) fb->buf, fb->len);
  esp_camera_fb_return(fb);
}