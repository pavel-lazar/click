#ifndef CLICK_STRINGMATCHER_HH
#define CLICK_STRINGMATCHER_HH
#include <click/element.hh>
#include "ahocorasickplus.hh"
CLICK_DECLS


class StringMatcher : public Element { 
public:
    StringMatcher() CLICK_COLD;
    ~StringMatcher() CLICK_COLD;

    const char *class_name() const		{ return "StringMatcher"; }
    const char *port_count() const    { return PORTS_1_1X2; }
    const char *processing() const    { return PROCESSING_A_AH; }
    // this element does not need AlignmentInfo; override Classifier's "A" flag
    const char *flags() const     { return ""; }
    bool can_live_reconfigure() const   { return true; }

    int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;
    void add_handlers() CLICK_COLD;

    Packet *simple_action(Packet *);

private:
    bool is_valid_patterns(Vector<String> &, ErrorHandler *); 
    static int write_handler(const String &, Element *e, void *thunk, ErrorHandler *errh) CLICK_COLD;
    AhoCorasick _matcher;
    Vector<String> _patterns;
    int _matches;
};


CLICK_ENDDECLS
#endif
