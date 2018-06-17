#pragma once

#include <ESP8266WiFi.h>

#define NTP_PACKET_SIZE 48

class TimeClient {

  private:
    float myUtcOffset = 0;
    long localEpoc = 0;
    long localMillisAtUpdate;
    
    const char* ntpServerName = "time.nist.gov";
    unsigned int localPort = 2390;
    
    byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

    
  public:
    TimeClient(float utcOffset);
    void updateTime();

    String getHours();
    String getMinutes();
    String getSeconds();
    String getFormattedTime();
    long getCurrentEpoch();
    long getCurrentEpochWithUtcOffset();

};

