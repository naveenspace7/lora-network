#include "lora_support.hpp"

using namespace std;

static const int CHANNEL = 0;
static const int INT_PIN = 7;
static const int RST = 0;


int initialize(long frequency) {
  digitalWrite(RST, LOW);
  delay(10);
  digitalWrite(RST, HIGH);
  delay(10);
  
  wiringPiSPISetup(CHANNEL, 500000);
  
  cout << "ID of LoRa board: " << (int) register_read(REG_VERSION) << endl;
  
  lora_sleep_mode();
  
  freq_config(frequency);
  
  register_write(REG_FIFO_TX_BASE_ADDR, 0);
  register_write(REG_FIFO_RX_BASE_ADDR, 0);
  register_write(REG_LNA, register_read(REG_LNA) | 0x03);
  register_write(REG_MODEM_CONFIG_3, 0x04);
  
  setTxPower(17);
  lora_stby_mode();

  register_write(REG_DIO_MAPPING_1, 0x00);
  // register_write(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_SINGLE);
  cout << "OP_MODE_REG: " << hex << (int) register_read(REG_OP_MODE) << endl;
  return 1;
}

void register_write(uint8_t addr, uint8_t val) {
    // make SPI write
    handle_transfer(addr | 0x80, val);
}

uint8_t register_read(uint8_t addr) {
    // make SPI write
    return handle_transfer(addr & 0x7f, 0x00);
}

uint8_t handle_transfer(uint8_t addr, uint8_t val) {

    unsigned char buffer[5] = {(unsigned char) addr, (unsigned char) val, 0x00};

    wiringPiSPIDataRW(CHANNEL, buffer, 2);
    val = (uint8_t) buffer[1];
    // delay(500);
    return val;
}

void binary_print(uint8_t temp) {
  bitset<8> x(temp);
  cout << x << endl;
}

void lora_sleep_mode() {
    // cout << "Entering sleep mode... ";
    register_write(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_SLEEP);
    // binary_print(register_read(REG_OP_MODE));
}

void lora_stby_mode() {
    // cout << "Entering standby mode... ";
    register_write(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
    // binary_print(register_read(REG_OP_MODE));
}

void freq_config(long frequency) {

  uint64_t frf = ((uint64_t)frequency << 19) / 32000000;
  // cout << "this: " << hex << (long) frf << endl;
  register_write(REG_FRF_MSB, (uint8_t)(frf >> 16));
  // cout << (int) register_read(REG_FRF_MSB) << " ? " << (int)(frf >> 16) << endl;
  register_write(REG_FRF_MID, (uint8_t)(frf >> 8));
  // cout << (int) register_read(REG_FRF_MID) << " ? " << (int)((uint8_t)(frf >> 8)) << endl;
  register_write(REG_FRF_LSB, (uint8_t)(frf >> 0));  
  // cout << (int) register_read(REG_FRF_LSB) << " ? " << (int)((uint8_t)(frf >> 0)) << endl;
}

void setOCP(uint8_t mA) {
  uint8_t ocpTrim = 27;

  if (mA <= 120) {
    ocpTrim = (mA - 45) / 5;
  } else if (mA <=240) {
    ocpTrim = (mA + 30) / 10;
  }

  register_write(REG_OCP, 0x20 | (0x1F & ocpTrim));
}

void setTxPower(int level) {
    // PA BOOST
    if (level > 17) {
      if (level > 20) {
        level = 20;
      }

      level -= 3;

      // High Power +20 dBm Operation (Semtech SX1276/77/78/79 5.4.3.)
      register_write(REG_PA_DAC, 0x87);
      setOCP(140);
    } 
  else {
      if (level < 2) {
        level = 2;
      }
      //Default value PA_HF/LF or +17dBm
      register_write(REG_PA_DAC, 0x84);
      setOCP(100);
    }
    register_write(REG_PA_CONFIG, PA_BOOST | (level - 2));
}

void Init_gpio_spi() {
  wiringPiSetup();
  pinMode(RST, OUTPUT);
  pinMode(INT_PIN, INPUT);
  pullUpDnControl(INT_PIN, PUD_DOWN);
  initialize(868000000);
  wiringPiISR(INT_PIN, INT_EDGE_RISING, received_data);
}

int TXbegin() {
  if (txInProgress()) return 0;

  lora_stby_mode();

  // explicitHeaderMode
  register_write(REG_MODEM_CONFIG_1, register_read(REG_MODEM_CONFIG_1) & 0xfe);

  // reset FIFO address and paload length
  register_write(REG_FIFO_ADDR_PTR, 0);
  register_write(REG_PAYLOAD_LENGTH, 0);

  return 1;
}

bool txInProgress() {
  if ((register_read(REG_OP_MODE) & MODE_TX) == MODE_TX) {
    return true;
  }

  if (register_read(REG_IRQ_FLAGS) & IRQ_TX_DONE_MASK) {
    // clear IRQ's
    register_write(REG_IRQ_FLAGS, IRQ_TX_DONE_MASK);
  }

  return false;
}

void sendData(uint8_t* txData, uint8_t size) {
  if (size > 255) size = 255;
  for(int i = 0; i < size; i++) register_write(REG_FIFO, txData[i]);
  register_write(REG_PAYLOAD_LENGTH, size);
}

int transmitNow() {
  register_write(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_TX);
  bitset<8> x(register_read(REG_OP_MODE));
  cout << "TXing now " << x;
  
  while((register_read(REG_IRQ_FLAGS) & IRQ_TX_DONE_MASK) == 0);
  
  cout << " .... TX Done" << endl;
  register_write(REG_IRQ_FLAGS, IRQ_TX_DONE_MASK);
  return 1;
}

void transmitter() {
  uint8_t payload[] = {1,4,3,7};
  while(1)
  {
    TXbegin();
    sendData(payload, 4);
    transmitNow();
    delay(5000);
  }
}