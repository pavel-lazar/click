#ifndef ETTMETRIC_HH
#define ETTMETRIC_HH
#include <click/element.hh>
#include "linkmetric.hh"
#include <click/hashmap.hh>
#include <click/etheraddress.hh>
#include <clicknet/wifi.h>
CLICK_DECLS

/*
 * =c
 * ETTMetric(LinkStat, LinkStat)
 * =s Wifi
 * =io
 * None
 *
 */



inline unsigned ett_metric(int ack_prob, int data_prob, int data_rate) 
{
  
  if (!ack_prob || ! data_prob) {
    return 0;
  }
  int retries = 100 * 100 * 100 / (ack_prob * data_prob) - 100;
  unsigned low_usecs = calc_usecs_wifi_packet(1500, data_rate, retries/100);
  unsigned high_usecs = calc_usecs_wifi_packet(1500, data_rate, (retries/100) + 1);

  unsigned diff = retries % 100;
  unsigned average = (diff * high_usecs + (100 - diff) * low_usecs) / 100;
  return average;

}

class IPOrderedPair {
public:
  IPAddress _a;
  IPAddress _b;
  
  IPOrderedPair() { }
  IPOrderedPair(IPAddress a, IPAddress b) {
    if (a.addr() > b.addr()) {
      _a = b;
      _b = a;
    } else {
      _a = a;
      _b = b;
    }
  }
  IPOrderedPair(const IPOrderedPair &p) : _a(p._a), _b(p._b) { }

  bool first(IPAddress x) {
    return (_a == x);
  }

  inline bool
  operator==(IPOrderedPair other)
  {
    return (other._a == _a && other._b == _b);
  }

};


inline unsigned
hashcode(IPOrderedPair p) 
{
  return hashcode(p._a) + hashcode(p._b);
}
class ETTStat;

class ETTMetric : public LinkMetric {
  
public:

  ETTMetric();
  ~ETTMetric();
  const char *class_name() const { return "ETTMetric"; }
  const char *processing() const { return AGNOSTIC; }

  int configure(Vector<String> &, ErrorHandler *);
  void add_handlers();

  void *cast(const char *);

  static String read_stats(Element *xf, void *);

  Vector <IPAddress> get_neighbors();
  void update_link(IPAddress from, IPAddress to, 
		   unsigned fwd, unsigned rev,
		   int fwd_rate, int rev_rate,
		   uint32_t seq);

  int get_tx_rate(EtherAddress);

private:
  class ETTStat *_ett_stat;
  class LinkTable *_link_table;

};

CLICK_ENDDECLS
#endif
