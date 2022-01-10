/*
 * Simple interface to the Fly Sky IBus RC system.
 */

#include <Arduino.h>
#include "FlySkyIBus.h"

void FlySkyIBus::begin(HardwareSerial& serial)
{
  serial.begin(115200);
  begin((Stream&)serial);
}

void FlySkyIBus::begin(Stream& stream)
{
  this->stream = &stream;
  this->state = DISCARD;
  this->last = millis();
  this->ptr = 0;
  this->len = 0;
  this->chksum = 0;
  this->lchksum = 0;
}

bool FlySkyIBus::loop(void)
{
  while (stream->available() > 0)
  {
    uint32_t now = millis();
    if (now - last >= PROTOCOL_TIMEGAP)
    {
      state = GET_LENGTH;
    }
    last = now;
    
    uint8_t v = stream->read();
    switch (state)
    {
      case GET_LENGTH:
        if (v == PROTOCOL_LENGTH)
        {
          ptr = 0;
          len = v - PROTOCOL_OVERHEAD;
          chksum = 0xFFFF - v;
          state = GET_DATA;
        }
        else
        {
          state = DISCARD;
        }
        break;

      case GET_DATA:
        buffer[ptr++] = v;
        chksum -= v;
        if (ptr == len)
        {
          state = GET_CHKSUML;
        }
        break;
        
      case GET_CHKSUML:
        lchksum = v;
        state = GET_CHKSUMH;
        break;

      case GET_CHKSUMH:
        // Validate checksum
        if (chksum == (v << 8) + lchksum)
        {
          // Execute command - we only know command 0x40
          switch (buffer[0])
          {
            case PROTOCOL_COMMAND40:
              // Valid - extract channel data
			  for (int i=0; i < PROTOCOL_CHANNELS; i++) {
				  channel[i] = (buffer[2*(i+1)] <<8) | (buffer[(2*i)+1]);
			  }
			  return true;
              break;

            default:
              break;
          }
        }
        state = DISCARD;
        break;

      case DISCARD:
      default:
        break;
    }
  }
  return false;
}

uint16_t FlySkyIBus::readChannel(uint8_t channelNr)
{
  if (channelNr < PROTOCOL_CHANNELS)
  {
    return channel[channelNr];
  }
  else
  {
    return 0;
  }
}
