#include <click/config.h>
#include "stringmatcher.hh"
#include <click/glue.hh>
#include <click/error.hh>
#include <click/confparse.hh>
#include <click/router.hh>
CLICK_DECLS

StringMatcher::StringMatcher() : _matches(0) {
}

StringMatcher::~StringMatcher() {
}

int StringMatcher::configure(Vector<String> &conf, ErrorHandler *errh)
{
    // This check will prevent us from doing any changes to the state if there is an error
    if (!is_valid_patterns(conf, errh)) {
        return -1; 
    }

    // if we are reconfiguring we need to reset the patterns and the matcher
    if ((!_matcher.is_open()) || _patterns.size()) {
        _matcher.reset();
        _patterns.clear();
    }

    for (int i=0; i < conf.size(); ++i) {
        // All patterns should be OK so we can only have duplicates 
        if (_matcher.add_pattern(conf[i], i)) {
            errh->warning("Pattern #d is a duplicate");
        } else {
            _patterns.push_back(conf[i]);        
        }
    }

    _matcher.finalize();


    if (!errh->nerrors()) {
        return 0;
    } else {
        return -1;
    }
}

bool StringMatcher::is_valid_patterns(Vector<String> &patterns, ErrorHandler *errh) {
    bool valid = true;
    AhoCorasick matcher; 
    for (int i=0; i<patterns.size(); ++i) {
        AhoCorasick::EnumReturnStatus rv = matcher.add_pattern(patterns[i], i);
        switch (rv) {
            case AhoCorasick::RETURNSTATUS_ZERO_PATTERN:
                errh->error("Pattern #%d has zero length", i);
                valid = false;
                break;
            case AhoCorasick::RETURNSTATUS_LONG_PATTERN:
                errh->error("Pattern #%d is too long", i);
                valid = false;
                break;
            case AhoCorasick::RETURNSTATUS_FAILED:
                errh->error("Pattern #%d had unknown error", i);
                valid = false;
                break;
            default:
                break;
        }
    }

    return valid;
}

Packet* StringMatcher::simple_action(Packet *p) {
    if (_matcher.match_any(p, false)) {
        _matches++;

        // push to port 1 if anything is connected
        if (noutputs() == 2) {
            output(1).push(p);
        } else {
            p->kill();
        }
        return 0;
    } else {
        // push to port 0 
        return p; 
    }
}

int
StringMatcher::write_handler(const String &, Element *e, void *, ErrorHandler *) {
    StringMatcher * string_matcher = static_cast<StringMatcher *>(e);
    string_matcher->_matches = 0;
    return 0;    
}

void StringMatcher::add_handlers() {
    add_data_handlers("matches", Handler::h_read, &_matches);
    add_write_handler("reset_count", write_handler, 0, Handler::h_button | Handler::h_nonexclusive);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(StringMatcher)
ELEMENT_REQUIRES(userlevel AhoCorasick)
ELEMENT_MT_SAFE(StringMatcher)
