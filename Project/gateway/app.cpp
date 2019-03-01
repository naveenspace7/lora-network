#include <errno.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <bitset>
#include <iostream>
#include <stdint.h>
#include <ctime>
#include <sstream>
#include <vector>

#include "lora.hpp"
#include "lora_support.hpp"
#include "packet.hpp"
#include "lora_db.hpp"
#include "logger.hpp"
/* TODO:
 * group all the fifo reads into one single transfer (hint: make use of heap to store data there)
 */ 

using namespace std;

vector<int> get_timestamp();


int _packetIndex = 0;

int parsePacket() {
  int packetLength = 0;
  int irqFlags = register_read(REG_IRQ_FLAGS);

  int temp_reg = register_read(REG_OP_MODE);
  // if (irqFlags != 0)
    // cout << "REG_IRQ_FLAGS : 0x" << hex << irqFlags << endl;

  // explicitHeaderMode
  register_write(REG_MODEM_CONFIG_1, register_read(REG_MODEM_CONFIG_1) & 0xfe);
  register_write(REG_IRQ_FLAGS, irqFlags);

  if ((irqFlags & IRQ_RX_DONE_MASK) && (irqFlags & IRQ_PAYLOAD_CRC_ERROR_MASK) == 0) {
    // received a packet
    _packetIndex = 0;

    // read packet length
    packetLength = register_read(REG_RX_NB_BYTES);

    // set FIFO address to current RX address
    register_write(REG_FIFO_ADDR_PTR, register_read(REG_FIFO_RX_CURRENT_ADDR));

    // put in standby mode
    lora_stby_mode();
    // cout << "parsing received packet of size : " << packetLength << endl;
  } 
  else if (register_read(REG_OP_MODE) != (MODE_LONG_RANGE_MODE | MODE_RX_SINGLE)) {
    // not currently in RX mode

    // reset FIFO address
    register_write(REG_FIFO_ADDR_PTR, 0);

    // put in single RX mode
    register_write(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_SINGLE);
  }
  return packetLength;
}

int avail() {
  return (register_read(REG_RX_NB_BYTES) - _packetIndex);
}

uint8_t readData() {
  if (!avail()) return -1;

  _packetIndex++;

  return register_read(REG_FIFO);
}

// receiving with polling method
void receiver(sql_cls& sql_handler) {

  while(1)
  {
    int packetSize = parsePacket();
    if(packetSize)
    {
      int i = 0;
      uint8_t *data = new uint8_t[packetSize];
      cout << "Received packet " << packetSize << " ";
      while(avail())
      {
        if (i > packetSize)
        {
          // log message and break
          cout << "Length is exceeding, skipping assignment." << endl;
          break;
        }
        data[i] = (uint8_t) readData();
        i++;
        // cout << (int) readData() << ' ';
      }
      // for (int j = 0; j < packetSize; j++) cout << hex << (int) data[j] << ' ';
      packet_data my_packet(data);
      vector<int> temp = get_timestamp();
      
      cout << temp[0] << " : " << temp[1] << " . " << temp[2] << " { " << my_packet.m_lid << ' ' << my_packet.m_sid << ' ' << my_packet.m_int_value << " }" << endl;

      // TODO: check if the table exists
      
      // cout << "packet:" << my_packet.m_lid << ' ' << my_packet.m_sid << ' ' << my_packet.m_int_value << " @ " << temp[0] << ':' << temp[1] << '.' << temp[2] << endl;

      // // stmt->execute("INSERT INTO books(title, price) VALUES ('book is this',43)");
      stringstream query;
      query << "INSERT INTO m03y2019(date, location_id, sensor_id, hour, min, value) "; 
      query << "VALUES(" << temp[3] << ", " << my_packet.m_lid << ", " << my_packet.m_sid <<  ", " << temp[0] << ", " << temp[1] << " , " << my_packet.m_int_value << ')';
      sql_handler.update_table(query.str());
      cout << query.str() << endl;
      delete[] data;
    }
  }
}

// CREATE table m02y2019(date INT, time VARCHAR(10), location_id INT, sensor_id INT, value INT)

// ISR for data received
void received_data()
{
  // cout << "received a packet I guess" << endl;
}

vector<int> get_timestamp()
{
  vector<int> temp(4);
  time_t now = time(0);
  struct tm *mytime = localtime(&now);
  temp[0] = (int) mytime->tm_hour;
  temp[1] = (int) mytime->tm_min;
  temp[2] = (int) mytime->tm_sec;
  temp[3] = (int) mytime->tm_mday;
  return temp;
}

int main()
{
  // logger log;
  
  sql_cls mysql("localhost", "root", "samaritan3");

  Init_gpio_spi();

  // transmitter();

  receiver(mysql);

  // log.exit_logger();
  return 0;
}