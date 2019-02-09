# LoRa Sensor Network

LoRa gateway is hosted on an SBC connected to internet and running Linux. LoRa sensor nodes are based on Arduino microcontrollers which contain different sensors attached.

Each LoRa sensor node captures the sensor reading at a defined interval and transmits it to the gateway. The gateway, up on receiving the information, moves the data into a database maintained in MySQL server. Applications wanting to use the information simply query the database with MySQL APIs.
