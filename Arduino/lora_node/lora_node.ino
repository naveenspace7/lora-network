#include <TimerOne.h>
#include <dht.h>
#include <SPI.h>

uint8_t ss = 10, reset = 9, DPIN = 7; // this should be changed according to the board

//#define TICK_INTERVAL 60000000 // this is 1 minute
#define TICK_INTERVAL 10000000

// registers
#define REG_FIFO                 0x00
#define REG_OP_MODE              0x01
#define REG_FRF_MSB              0x06
#define REG_FRF_MID              0x07
#define REG_FRF_LSB              0x08
#define REG_PA_CONFIG            0x09
#define REG_OCP                  0x0b
#define REG_LNA                  0x0c
#define REG_FIFO_ADDR_PTR        0x0d
#define REG_FIFO_TX_BASE_ADDR    0x0e
#define REG_FIFO_RX_BASE_ADDR    0x0f
#define REG_FIFO_RX_CURRENT_ADDR 0x10
#define REG_IRQ_FLAGS            0x12
#define REG_RX_NB_BYTES          0x13
#define REG_PKT_SNR_VALUE        0x19
#define REG_PKT_RSSI_VALUE       0x1a
#define REG_MODEM_CONFIG_1       0x1d
#define REG_MODEM_CONFIG_2       0x1e
#define REG_PREAMBLE_MSB         0x20
#define REG_PREAMBLE_LSB         0x21
#define REG_PAYLOAD_LENGTH       0x22
#define REG_MODEM_CONFIG_3       0x26
#define REG_FREQ_ERROR_MSB       0x28
#define REG_FREQ_ERROR_MID       0x29
#define REG_FREQ_ERROR_LSB       0x2a
#define REG_RSSI_WIDEBAND        0x2c
#define REG_DETECTION_OPTIMIZE   0x31
#define REG_INVERTIQ             0x33
#define REG_DETECTION_THRESHOLD  0x37
#define REG_SYNC_WORD            0x39
#define REG_INVERTIQ2            0x3b
#define REG_DIO_MAPPING_1        0x40
#define REG_VERSION              0x42
#define REG_PA_DAC               0x4d

// modes
#define MODE_LONG_RANGE_MODE     0x80
#define MODE_SLEEP               0x00
#define MODE_STDBY               0x01
#define MODE_TX                  0x03
#define MODE_RX_CONTINUOUS       0x05
#define MODE_RX_SINGLE           0x06

// PA config
#define PA_BOOST                 0x80

// IRQ masks
#define IRQ_TX_DONE_MASK           0x08
#define IRQ_PAYLOAD_CRC_ERROR_MASK 0x20
#define IRQ_RX_DONE_MASK           0x40

#define MAX_PKT_LENGTH           255

dht sensor;

uint8_t l_id = 5;

uint32_t tick = 1;
bool skip_frame = false;

const int rate_temp_sensor = 5, rate_humid_sensor = 5, rate_mq135_sensor = 2; // 10 mins
int sensorVal, digitalVal;
const uint8_t temp_s_id = 12, hum_s_id = 13, mq135_s_id = 18;

int sort_desc(const void *cmp1, const void *cmp2) {
  // Need to cast the void * to int *
  int a = *((int *)cmp1);
  int b = *((int *)cmp2);
  // The comparison
  return a > b ? -1 : (a < b ? 1 : 0);
  // A simpler, probably faster way:
  //return b - a;
}

void timer_tick() {
  tick++;
  //  Serial.println("hw");
  if ((tick % rate_temp_sensor) == 0) tx_temp_data();
  if ((tick % rate_humid_sensor) == 0) tx_hmd_data();
  if ((tick % rate_mq135_sensor) == 0) tx_mq135_data();
}

void tx_mq135_data() {
  uint16_t mq135_value = 0;
  uint16_t meas[5];
  for(int i = 0; i < 5; i++) {
    meas[i] = analogRead(0);
    qsort(meas, 5, sizeof(uint16_t), sort_desc);
  }
  mq135_value = meas[2];
  Serial.print("MQ-135:");
  uint8_t packet[] = {l_id, mq135_s_id, sizeof(mq135_value), (uint8_t)((mq135_value & 0xff00) >> 8), (uint8_t)(mq135_value & 0xff)};
  Serial.println(mq135_value);
  TXbegin();
  sendData(packet, 5);
  transmitNow();
}

void tx_temp_data() {
  uint8_t temperature_value = 0;
  uint8_t meas[5];
  for(int i = 0; i < 5; i++)   {
    int chk = sensor.read11(DPIN);
    meas[i] = (byte) sensor.temperature;
    qsort(meas, 5, sizeof(byte), sort_desc);
  }
  temperature_value = meas[2];
  Serial.print("Temperature:");
  uint8_t packet[] = {l_id, temp_s_id, sizeof(temperature_value), temperature_value};
  Serial.println(temperature_value);
  TXbegin();
  sendData(packet, 4);
  transmitNow();
}

void tx_hmd_data() {
  uint8_t humidity_value = 0;
  uint8_t meas[5];
  for(int i = 0; i < 5; i++)   {
    int chk = sensor.read11(DPIN);
    meas[i] = (byte) sensor.humidity;
    qsort(meas, 5, sizeof(byte), sort_desc);
  }
  humidity_value = meas[2];
  Serial.print("Humidity:");
  uint8_t packet[] = {l_id, hum_s_id, sizeof(humidity_value), humidity_value};
  Serial.println(humidity_value);
  TXbegin();
  sendData(packet, 4);
  transmitNow();
}

uint8_t readRegister(uint8_t addr) {
  return singleTransfer(addr & 0x7f, 0);
}

void writeRegister(uint8_t addr, uint8_t val) {
  singleTransfer(addr | 0x80, val);
}

uint8_t singleTransfer(uint8_t addr, uint8_t val) {
  uint8_t resp;
  
  digitalWrite(ss, LOW);
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  SPI.transfer(addr);
  resp = SPI.transfer(val);
  SPI.endTransaction();
  digitalWrite(ss, HIGH);
  
  return resp;
}

void enter_sleep() {
  Serial.print("Entering sleep mode... ");
  writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_SLEEP);
  Serial.println(readRegister(REG_OP_MODE), BIN);
}

void enter_idle() {
  Serial.print("Entering standby mode... ");
  writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
  Serial.println(readRegister(REG_OP_MODE), BIN);
}

void frequency_config(long frequency) {

  //_frequency = frequency;

  uint64_t frf = ((uint64_t)frequency << 19) / 32000000;

  writeRegister(REG_FRF_MSB, (uint8_t)(frf >> 16));
  writeRegister(REG_FRF_MID, (uint8_t)(frf >> 8));
  writeRegister(REG_FRF_LSB, (uint8_t)(frf >> 0));  
}

void setOCP(uint8_t mA) {
  uint8_t ocpTrim = 27;

  if (mA <= 120) {
    ocpTrim = (mA - 45) / 5;
  } else if (mA <=240) {
    ocpTrim = (mA + 30) / 10;
  }

  writeRegister(REG_OCP, 0x20 | (0x1F & ocpTrim));
}

void setTxPower(int level) {
    // PA BOOST
    if (level > 17) {
      if (level > 20) {
        level = 20;
      }

      level -= 3;

      // High Power +20 dBm Operation (Semtech SX1276/77/78/79 5.4.3.)
      writeRegister(REG_PA_DAC, 0x87);
      setOCP(140);
    } 
  else {
      if (level < 2) {
        level = 2;
      }
      //Default value PA_HF/LF or +17dBm
      writeRegister(REG_PA_DAC, 0x84);
      setOCP(100);
    }
    writeRegister(REG_PA_CONFIG, PA_BOOST | (level - 2));
}

int initialize(long frequency) {
  pinMode(ss, OUTPUT);
  digitalWrite(ss, HIGH);
    pinMode(reset, OUTPUT);
  digitalWrite(reset, LOW);
  delay(10);
  digitalWrite(reset, HIGH);
  delay(10);
  
  SPI.begin();

  delay(10);
  
  SPI.begin();
  
  Serial.print("ID of LoRa board: ");
  Serial.println(readRegister(REG_VERSION));
  
  enter_sleep();
  
  frequency_config(frequency);
  
  writeRegister(REG_FIFO_TX_BASE_ADDR, 0);
  writeRegister(REG_FIFO_RX_BASE_ADDR, 0);
  writeRegister(REG_LNA, readRegister(REG_LNA) | 0x03);
  writeRegister(REG_MODEM_CONFIG_3, 0x04);
  
  setTxPower(17);
  enter_idle();
  writeRegister(REG_DIO_MAPPING_1, 0x00);
  return 1;
}

bool txInProgress() {
  if ((readRegister(REG_OP_MODE) & MODE_TX) == MODE_TX) {
    return true;
  }

  if (readRegister(REG_IRQ_FLAGS) & IRQ_TX_DONE_MASK) {
    // clear IRQ's
    writeRegister(REG_IRQ_FLAGS, IRQ_TX_DONE_MASK);
  }

  return false;
}

int TXbegin() {
  if (txInProgress()) return 0;

  enter_idle();

  // explicitHeaderMode
  writeRegister(REG_MODEM_CONFIG_1, readRegister(REG_MODEM_CONFIG_1) & 0xfe);

  // reset FIFO address and paload length
  writeRegister(REG_FIFO_ADDR_PTR, 0);
  writeRegister(REG_PAYLOAD_LENGTH, 0);

  return 1;
}

void sendData(uint8_t* txData, byte size) {
  if (size > 255) size = 255;
  for(int i = 0; i < size; i++) writeRegister(REG_FIFO, txData[i]);
  writeRegister(REG_PAYLOAD_LENGTH, size);
}

int transmitNow() {
  writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_TX);
  Serial.print("TXing now ");
  Serial.print(readRegister(REG_OP_MODE), BIN);
  
  while((readRegister(REG_IRQ_FLAGS) & IRQ_TX_DONE_MASK) == 0);
  
  Serial.println(" .... TX Done");
  writeRegister(REG_IRQ_FLAGS, IRQ_TX_DONE_MASK);
  return 1;
}

void setup() {
  Serial.begin(9600);
  Timer1.initialize(TICK_INTERVAL); // 1 minute
  Timer1.attachInterrupt(timer_tick);

  if (!initialize(868000000))
  {
    Serial.println("Failed to init");
    while(1);
  }
}

void loop() {

  // uint8_t packet[] = {1, 4, 3, 7};
  // TXbegin();
  // sendData(packet, 4);
  // transmitNow();
  // delay(60000);

  //  if ((tick % temp_sensor) == 0)
  //  {tx_temp_data();}
  //  if ((tick % humid_sensor) == 0)
  //  {tx_hmd_data();skip_frame = true;}
  //  if (skip_frame) {tick++; skip_frame = false;}
}
