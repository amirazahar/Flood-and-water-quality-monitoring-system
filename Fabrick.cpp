#include "Fabrick.h"

Fabrick::Fabrick()
{
  // Nothing here
}

//***********************************
// Start LoRa Module
//***********************************
void Fabrick::lora_begin(SoftwareSerial *lora_ss)
{
  // Software serial for LoRa module	
  _lora_ss = lora_ss;
  _lora_ss->begin(9600);
}

//***********************************
// Start LoRa DK
//***********************************
void Fabrick::lora_dk_begin(SoftwareSerial *lora_ss, Stream *debug_s)// DHT22 *dht, Ultrasonic *ultrasonic, SoftwareSerial *pm25_ss
{
  // Debug Serial
  _debug_s = debug_s;	
	
  // Software serial for LoRa module	
  _lora_ss = lora_ss;
  _lora_ss->begin(9600);
}

//***********************************
// Start WiFi DK
//***********************************
void Fabrick::wifi_dk_begin(SoftwareSerial *wifi_ss, PubSubClient *mqtt_client, String ssid, String pass, Stream *debug_s)//DHT22 *dht, Ultrasonic *ultrasonic, SoftwareSerial *pm25_ss 
{
  // Note: Have to declare the sensors first before WiFi
  // Debug Serial
  _debug_s = debug_s;
  
  // MQTT Client
  _mqtt_client = mqtt_client; 	
  
  // Software serial for WiFi shield
  _wifi_ss = wifi_ss;
  _wifi_ss->begin(9600);
    
  // Initialize ESP module
  WiFi.init(_wifi_ss);
  
  // Check for the presence of the WiFi shield
  if (WiFi.status() == WL_NO_SHIELD) {
    _debug_s->println(F("WiFi shield not present"));
    // don't continue
    while (true);
  }
  
  // Convert SSID and password from String to Char 
  int ssid_len = int(ssid.length());
  int pass_len = int(pass.length());
  char ssid_c[ssid_len+1];
  char pass_c[pass_len+1];
  ssid.toCharArray(ssid_c,ssid_len+1);
  pass.toCharArray(pass_c,pass_len+1);
   
  // Attempt to connect to WiFi network
  int status  = WL_IDLE_STATUS; // WiFi status
  while ( status != WL_CONNECTED) {
    _debug_s->print(F("Attempting to connect to WPA SSID: "));
    _debug_s->println(ssid_c);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid_c, pass_c);
  }
  // Connected to WiFi network
  _debug_s->println(F("Connected to the WiFi network"));
  
  // Print WiFi status
  _debug_s->print(F("SSID: "));
  _debug_s->println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  _debug_s->print(F("IP Address: "));
  _debug_s->println(ip);      
  
}

//***********************************
// Get waterLevel_Data
//***********************************
String Fabrick::get_waterLevel_Data()
{
  // Get the average value of water level data
  float sum        = 0;
  for(int i=0; i<32; i++){
      sum += analogRead(pinWaterLevel);
  }
  int waterLevel_Data = int(sum / 32);  
  
  // Convert to string 
  String waterLevel_Data_s = String(waterLevel_Data, HEX);
  
  // Debug print
  if (debug_mode==1)
  {
  	_debug_s->print(F("Water Level data: ")); 
  	_debug_s->println(waterLevel_Data);
  	//_debug_s->print("Water Level data in HEX: "); 
  	//_debug_s->println(waterLevel_Data_s);  	
  }
  
  return waterLevel_Data_s;
}

String Fabrick::get_phLevel_Data()
{
  // Get the average value of the PH level data
  float sum        = 0;
  for(int i=0; i<32; i++){
      sum += analogRead(pinPHlevel);
  }
  int phLevel_Data = int(sum / 32);  

  // Convert to string 
  String phLevel_Data_s = String(phLevel_Data, HEX);
  
  // Debug print
  if (debug_mode==1)
  {
  	_debug_s->print(F("pH Level data: ")); 
  	_debug_s->println(phLevel_Data);
  	//_debug_s->print("pH Level data in HEX: "); 
  	//_debug_s->println(phLevel_Data_s);  	
  }
  
  return phLevel_Data_s;
}


//***********************************
// LoRa Send Water Level Data
//***********************************
void Fabrick::lora_send_waterLevel(String device_id){
  int frame_cntr = 1;     // Frame cntr	
  int data_ch    = 0;     // Data channel
  int data_typ   = 3319;  // Data type
  int data_len   = 4;     // Payload length (it must be even number!)

  // Get the Water Level data
  String waterLevel_Data = get_waterLevel_Data();

  // Send the data via LoRa
  lora_send(device_id, frame_cntr, data_ch, data_typ, waterLevel_Data, data_len);

  // Duty cycle of ~1 minute
  delay(6000);
}
//***********************************
// LoRa Send pH Level Data
//***********************************
void Fabrick::lora_send_phLevel(String device_id){
  int frame_cntr = 1;     // Frame cntr	
  int data_ch    = 0;     // Data channel
  int data_typ   = 3326;  // Data type
  int data_len   = 4;     // Payload length (it must be even number!)

  // Get the pH Level data
  String phLevel_Data = get_phLevel_Data();

  // Send the data via LoRa
  lora_send(device_id, frame_cntr, data_ch, data_typ, phLevel_Data, data_len);

  // Duty cycle of ~1 minute
  delay(6000);
}

//***********************************
// WiFi Send Water Level Data
//***********************************
void Fabrick::wifi_send_waterLevel(String device_id, String fabrick_usr, String fabrick_pass, String pub_topic){
  int frame_cntr = 1;     // Frame cntr	
  int data_ch    = 0;     // Data channel
  int data_typ   = 3319;  // Data type
  int data_len   = 4;     // Payload length 

  // Ensure MQTT connection
  check_mqtt_connect(fabrick_usr, fabrick_pass);
  
  // Get the Water Level data
  String waterLevel_Data = get_waterLevel_Data();

  // Send the data via LoRa
  wifi_send(device_id, frame_cntr, data_ch, data_typ, waterLevel_Data, data_len, pub_topic);

  // Delay 10s
  delay(10000);
}

//***********************************
// WiFi Send pH Level Data
//***********************************
void Fabrick::wifi_send_phLevel(String device_id, String fabrick_usr, String fabrick_pass, String pub_topic){
  int frame_cntr = 1;     // Frame cntr	
  int data_ch    = 0;     // Data channel
  int data_typ   = 3326;  // Data type
  int data_len   = 4;     // Payload length 

  // Ensure MQTT connection
  check_mqtt_connect(fabrick_usr, fabrick_pass);
  
  // Get the pH Level data
  String phLevel_Data = get_phLevel_Data();

  // Send the data via LoRa
  wifi_send(device_id, frame_cntr, data_ch, data_typ, phLevel_Data, data_len, pub_topic);

  // Delay 10s
  delay(10000);
}

//***********************************
// Lora Send
//***********************************
void Fabrick::lora_send(String device_id, int frame_cntr, int data_ch, int data_typ, String data, int data_len_in_hex)
{
  // Please refer to IPSO Smart Objects Document for packet format.  
  
  // Device ID
  String device_id_s = device_id;
  
  // Frame counter
  // Convert to string and ensure length of 2 (in hex) or 1 byte
  String frame_cntr_s = adding_zeros(String(frame_cntr, HEX), 2);
  
  // Data channel
  // Convert to string and ensure length of 2 (in hex) or 1 byte
  String data_ch_s = adding_zeros(String(data_ch, HEX), 2);
  
  // Data type
  // Convert to string and ensure length of 2 (in hex) or 1 byte
  String data_type_s = adding_zeros(String(data_typ-3200, HEX), 2);
  
  // Payload data 
  // Ensure length of data_len_in_hex
  String data_s = adding_zeros(data, data_len_in_hex);
  
  // Tx packet
  String tx_packet = device_id_s + frame_cntr_s + data_ch_s + data_type_s + data_s;
 
  // Length of the Tx packet
  int tx_packet_len = int(tx_packet.length());
  
  // Initialisation
  char at_command[tx_packet_len+12];

  // Send data via LoRa
  String len_in_string = String(tx_packet_len);                                // Convert to string
  String at_command_s ="AT+DTX="+ len_in_string + "," + tx_packet + "\r\n";    // AT command in string
  at_command_s.toCharArray(at_command,tx_packet_len+12);                       // Convert to char
  _lora_ss->write(at_command);                                                 // AT command to LoRa
  
  // Debug mode print
  if (debug_mode){
  	_debug_s->print(F("LoRa send -> "));
    _debug_s->println(at_command_s);
    _debug_s->println(F(" "));
  }
}

//***********************************
// WiFi Send
//***********************************
void Fabrick::wifi_send(String device_id, int frame_cntr, int data_ch, int data_typ, String data, int data_len_in_hex, String pub_topic){
  // Please refer to IPSO Smart Objects Document for packet format.  
  
  // Device ID
  String device_id_s = device_id;
  
  // Frame counter
  // Convert to string and ensure length of 2 (in hex) or 1 byte
  String frame_cntr_s = adding_zeros(String(frame_cntr, HEX), 2);
  
  // Data channel
  // Convert to string and ensure length of 2 (in hex) or 1 byte
  String data_ch_s = adding_zeros(String(data_ch, HEX), 2);
  
  // Data type
  // Convert to string and ensure length of 2 (in hex) or 1 byte
  String data_type_s = adding_zeros(String(data_typ-3200, HEX), 2);
  
  // Payload data 
  // Ensure length of data_len_in_hex
  String data_s = adding_zeros(data, data_len_in_hex);
  
  // Tx packet
  String tx_packet = device_id_s + frame_cntr_s + data_ch_s + data_type_s + data_s;
 
  // Length of the Tx packet
  int tx_packet_len = int(tx_packet.length());
  
  // Initialisation
  char tx_packet_c[tx_packet_len+1];

  // Convert publish topic from String to Char 
  int pub_topic_len  = int(pub_topic.length());
  char pub_topic_c[pub_topic_len+1];
  pub_topic.toCharArray(pub_topic_c,pub_topic_len+1); 
  
  // Publish to MQTT server
  tx_packet.toCharArray(tx_packet_c,tx_packet_len+1);                    // Convert to char
  _mqtt_client->publish(pub_topic_c, tx_packet_c); // MQTT publish topic
  
  // Debug mode print
  if (debug_mode){
  	_debug_s->print(F("WiFi send -> "));
    _debug_s->println(tx_packet);
    _debug_s->println(" ");
  }
}

//***********************************
// Adding Zeros
//***********************************
String Fabrick::adding_zeros(String data, int len){
  
  // Length of the data 
  int data_len = data.length();
  
  // Adding zeros 
  String data_ii = data;
  if (data_len<len){
      for (int ii = 0; ii < len-data_len; ii++){
          data_ii = "0" + data_ii;
      }
  }

  return data_ii;
}

//***********************************
// Check MQTT Connection
//***********************************
void Fabrick::check_mqtt_connect(String fabrick_usr, String fabrick_pass){
	
  // Convert fabrick username and password from String to Char 
  int fusr_len  = int(fabrick_usr.length());
  int fpass_len = int(fabrick_pass.length());
  char fabrick_usr_c[fusr_len+1];
  char fabrick_pass_c[fpass_len+1];
  fabrick_usr.toCharArray(fabrick_usr_c,fusr_len+1);
  fabrick_pass.toCharArray(fabrick_pass_c,fpass_len+1);
  
  // Loop until we're reconnected
  while (!_mqtt_client->connected()) 
  {
    _debug_s->println(F("Attempting MQTT connection..."));
    // Attempt to connect
    if (_mqtt_client->connect("ESP8266Client", fabrick_usr_c, fabrick_pass_c))
    {
      _debug_s->println(F("Connected"));
    } else {
      _debug_s->print(F("failed, rc="));
      _debug_s->print(_mqtt_client->state());
      _debug_s->println(F(" try again in 5 seconds"));
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  _mqtt_client->loop();
}
