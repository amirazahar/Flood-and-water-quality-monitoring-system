#ifndef _Fabrick_H
#define _Fabrick_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "WiFiEsp.h"
#include <PubSubClient.h>


// Pins Assignment
const int debug_mode   =  1; //  (1=debug mode)
const int pinPHlevel   = A0; //  PH Level sensor pin assignment
const int pinWaterLevel= A1; //  Water Level sensor pin assignment
const int pinLoraTx    =  3; //  LoRa Tx pin assignment
const int pinLoraRx    =  2; //  LoRa Rx pin assignment
const int pinWifiTx    =  3; //  WiFi Tx pin assignment 
const int pinWifiRx    =  2; //  WiFi Rx pin assignment

// Class
class Fabrick
{
  public:
    Fabrick();
    void   lora_begin(SoftwareSerial *lora_ss);
    void   lora_dk_begin(SoftwareSerial *lora_ss, Stream *debug_s); 
    void   wifi_dk_begin(SoftwareSerial *wifi_ss, PubSubClient *mqtt_client, String ssid, String pass, Stream *debug_s); 
    String get_waterLevel_Data();
	String get_phLevel_Data();
    void   lora_send_waterLevel(String device_id);
	void   lora_send_phLevel(String device_id);
    void   wifi_send_waterLevel(String device_id, String fabrick_usr, String fabrick_pass, String pub_topic);
	void   wifi_send_phLevel(String device_id, String fabrick_usr, String fabrick_pass, String pub_topic);   
    void   lora_send(String device_id, int frame_cntr, int data_ch, int data_typ, String data, int data_len_in_hex);
    void   wifi_send(String device_id, int frame_cntr, int data_ch, int data_typ, String data, int data_len_in_hex, String pub_topic);
    String adding_zeros(String data, int len);
    void   check_mqtt_connect(String fabrick_usr, String fabrick_pass);
    
  private:
    SoftwareSerial *_lora_ss;
    SoftwareSerial *_wifi_ss;
    PubSubClient   *_mqtt_client;
    Stream         *_debug_s;
}; 

extern Fabrick fabrick;

#endif
