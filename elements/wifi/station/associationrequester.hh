#ifndef CLICK_ASSOCIATIONREQUESTER_HH
#define CLICK_ASSOCIATIONREQUESTER_HH
#include <click/element.hh>
#include <clicknet/ether.h>
CLICK_DECLS

/*
=c

AssociationRequester

=s Wifi, Wireless Station

Sends 802.11 association requests when poked.


=d 

=h bssid read/write
The bssid to associate to

=h eth read/write
The station's ethernet address

=h ssid read/write
The ssid to associate to

=h listen_interval read/write
The listen interval for the station, in milliseconds

=h associated read only
If a association response was received and its status equals 0, this will
be true.

=h send_assoc_req
Sends an association request based on values of the handlers.

=a BeaconScanner */

class AssociationRequester : public Element { public:
  
  AssociationRequester();
  ~AssociationRequester();

  const char *class_name() const	{ return "AssociationRequester"; }
  const char *processing() const	{ return PUSH; }
  
  int configure(Vector<String> &, ErrorHandler *);
  bool can_live_reconfigure() const	{ return true; }

  void push(int, Packet *);
  void send_assoc_req();

  void add_handlers();
  void reset();


  void process_response(Packet *p);
  void process_disassociation(Packet *p);
  bool _debug;


  bool _associated;
  EtherAddress _eth;
  EtherAddress _bssid;
  String _ssid;
  uint16_t _listen_interval;
  class AvailableRates *_rtable;

  String scan_string();
 private:



};

CLICK_ENDDECLS
#endif
