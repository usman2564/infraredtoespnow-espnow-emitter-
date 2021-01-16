#include <Arduino.h>
#include "WiFi.h"
#include <esp_now.h>

//CODE FOR THE MASTER ESP (Sender) -- By USMAN QURESHI



//Infrared Reciever Module class: input is the signal pin
class Sirc
{
  public:
  Sirc(int p)
  {
    recieverPin = p;
  }

  void initialize()
  {
    //the signal of a reciever is input
    pinMode(recieverPin, INPUT);
  }


//this process is the conversion of a signal into a code.
  int infrared()
  {
    //set the value of code as 0 before checking for a new code
    int code = 0;

    //Finding the startbit. Since the reciever is active low, we need to check for falling edges
    if(pulseIn(recieverPin, LOW) > 2000) 
    {

      //if we get a start bit, we determine the value of each bit of the 12bit code
      for(int i = 0; i <=11; i++) 
      {
        //a 1-bit means that we found detected a falling edge and a low signal longer than 1000 microseconds
        if(pulseIn(recieverPin, LOW) > 1000) 
        {
          //we use bit shifting from LSB to MSB (right to left) since the LSB is sent first in this protocol
          //basically we keep adding the value of each bit from the LSB to the MSB
          code = code + (1<<i);
        }
      }
    }

    //return our code value (if code = 0 we know that nothing was recieved)
    return code;
  }

  private:
  int recieverPin;
};


//attach IR Reciever to pin 21
Sirc sirc(21);


//this stores the MAC address for the reciever ESP
uint8_t recieverAddress[] = {0xFC, 0xF5,0xC4,0x0A,0x45,0xD8};

void setup()
{
  //initialize the pin
  sirc.initialize();
  //start serial monitor and check if the ESP NOW initialization is OK
  Serial.begin(9600);
  WiFi.mode(WIFI_MODE_STA);
  Serial.println("Controller MAC address:");
  Serial.println(WiFi.macAddress());
  if(esp_now_init() != ESP_OK) {
    Serial.println("esp-initialization failed");
    return;
  }

  //Pairing the sender ESP to the Reciever ESP on channel 0, data is not encrypted
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, recieverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  //check to see if pair succeeded or failed, result in serial monitor
  if(esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to pair!");
    return;
  }


}
 
void loop(){

//store our code, and send it to the paired reciever

int sendCode = sirc.infrared();
esp_err_t result = esp_now_send(recieverAddress, (uint8_t *) &sendCode, sizeof(int));

}