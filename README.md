EthShield
=========

Ethernet shield based on ATMega644 micro-controller and ENC28J60 network chip

Microcontroller protocols supported
===================================
- Two Wire (I2C)
- Universal Synchronous and Asymchronous serial Receiver and Transmitter (USART)
- Serial Peripheral Interface (SPI)

Network protocols supported
===========================
- Link layer
  - Adres Resolution Protocol (ARP, [rfc826](http://tools.ietf.org/html/rfc826))
- Internet layer
  - Internet Protocol (IP, [rfc791](http://tools.ietf.org/html/rfc791))
  - Internet Control Message Protocol (ICMP [rfc792](http://tools.ietf.org/html/rfc792))
- Transport Layer
  - User Datagram Protocol (UDP, [rfc768](http://tools.ietf.org/html/rfc768))
  - Transmission Control Protocol (TCP, transport layer, [rfc793](http://tools.ietf.org/html/rfc793), [draft 1323](http://tools.ietf.org/html/draft-ietf-tcpm-1323bis-21))
- Application layer
  - Dynamic Host Configuration Protocol (DHCP, [rfc2131](http://tools.ietf.org/html/rfc2131), [rfc1533](http://tools.ietf.org/html/rfc1533))
  - Hypertext Transfer Protocol (HTTP, application layer, [rfc2616](http://tools.ietf.org/html/rfc2616))


# Planned (randomly ordered)
- Network Time Protocol (NTP, application layer, [rfc958](http://tools.ietf.org/html/rfc958))
- Domain Name System (DNS, application layer, [rfc1035](http://tools.ietf.org/html/rfc1035))
- Extensible Messaging and Presence Protocol (XMPP, application layer, [rfc6120](http://tools.ietf.org/html/rfc6120))
