EthShield changelog
===================

0.12.7
------
- Program fuses slow, hex fast (new chips need slow programming)

0.12.6
------
- Add i2c_write_register
- Allow all tlc59116 registers to be set

0.12.5
------
- Fix method name typo

0.12.4
------
- Remove Xcode project

0.12.3
------
- Add type to i2c_speed in tlc59116

0.12.2
------
- Updated README
- Add links to used RFCs in README
- Ignore copies of RFCs on disk

0.12.1
------
- Removed xcode userdata from repo

0.12.0
------
- Basic support for Texas Instruments Led Driver (TLC59116) chip

0.11.1
------
- Fix I2C receive byte when sending NACK as reply
- Introduce I2C_WRITE and I2C_READ to set read/write bit in address

0.11.0
------
- Support I2C (TWI) protocol

0.10.0
------
- Add Werkti, which tracks network usage

0.9.1
-----
- Make ARP less verbose in debug mode

0.9.0
-----
- Support DHCP protocol

0.8.0
-----
- Support ARP protocol

0.7.2
-----
- Add UDP server to network_init and network_backbone

0.7.1
-----
- Fix typo in logger

0.7.0
-----
- Support UDP protocol

0.6.1
-----
- Fix logger level outputs

0.6.0
-----
- Rename debug to logger
- Add defines for debug and info level messages

0.5.0
-----
- Add uptime to keep track of running time

0.4.0
-----
- Add counter to keep track of seconds

0.3.0
-----
- Support network chip (uses SPI)

0.2.0
-----
- Clean Makefile
- Support SPI protocol

0.1.0
-----
- Support USART protocol
- Support debug over UART
