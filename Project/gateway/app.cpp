#include <errno.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <bitset>
#include <iostream>
#include <stdint.h>
#include "lora.hpp"
#include "lora_support.hpp"
#include "packet.hpp"
#include <ctime>
#include <sstream>
#include <vector>
#include "sql_update.hpp"
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
    cout << "parsing received packet of size : " << packetLength << endl;
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
      uint8_t *data = new uint8_t(packetSize);
      cout << "Received packet: ";
      while(avail())
      {
        data[i] = (uint8_t) readData();
        i++;
        // cout << (int) readData() << ' ';
      }
      // for (int j = 0; j < packetSize; j++) cout << hex << (int) data[j] << ' ';
      packet_data my_packet(data);
      cout << "packet:" << my_packet.m_lid << ' ' << my_packet.m_sid << ' ' << my_packet.m_int_value << ' ';

      // stmt->execute("INSERT INTO books(title, price) VALUES ('wtf is this',43)");
      vector<int> temp = get_timestamp();
      stringstream query;
      query << "INSERT INTO m122018(date, time, lid, sid, val) "; 
      query << "VALUES( 3, '" << temp[1] << ':' << temp[2] << "' , " << my_packet.m_lid << ", " << my_packet.m_sid << ", " << my_packet.m_int_value << ')';
      sql_handler.update_table(query.str());
      cout << query.str() << endl;
      delete[] data;
    }
  }
}

// void get_timestamp()
// {
//   vector<int> temp(3);
//   // temp[]
//   time_t now = time(0);
//   struct tm *mytime = localtime(&now);
// }

// ISR for data received
void received_data()
{
  // cout << "received a packet I guess" << endl;
}

vector<int> get_timestamp()
{
  vector<int> temp(3);
  // temp[]
  time_t now = time(0);
  struct tm *mytime = localtime(&now);
  cout << mytime->tm_sec << ' ' << mytime->tm_min << ' ' << endl;
  temp[0] = mytime->tm_hour;
  temp[1] = mytime->tm_min;
  temp[2] = mytime->tm_sec;
  return temp;
}

int main()
{
  get_timestamp();

  cout << "hello world" << endl;
  sql_cls mysql("localhost", "root", "samaritan3");

  Init_gpio_spi();

  // transmitter();

  receiver(mysql);

  return 0;
}