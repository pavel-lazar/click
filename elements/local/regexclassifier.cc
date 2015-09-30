#include <click/config.h>
#include "regexclassifier.hh"
#include <click/glue.hh>
#include <click/error.hh>
#include <click/confparse.hh>
#include <click/router.hh>

CLICK_DECLS
RegexClassifier::RegexProgram::RegexProgram() : 
_compiled(false), 
_compiled_regex(new re2::RE2::Set(re2::RE2::Options(), re2::RE2::UNANCHORED))
{
  
}

RegexClassifier::RegexProgram::~RegexProgram() {
    if (_compiled_regex) {
        delete _compiled_regex;
    }
}

void RegexClassifier::RegexProgram::copy(const RegexProgram& other) {
    if (!_patterns.empty()) {
        _patterns.clear();
    } 

    if (_compiled_regex) {
        delete _compiled_regex;
    }
    _compiled_regex = new re2::RE2::Set(re2::RE2::Options(), re2::RE2::UNANCHORED);
    for (int i=0; i < other._patterns.size(); i++) {
        add(other._patterns[i]);
    }

    if (other._compiled) {
        compile();    
    }

}

RegexClassifier::RegexClassifier() {
}

RegexClassifier::~RegexClassifier() {
}

int RegexClassifier::configure(Vector<String> &conf, ErrorHandler *errh)
{
    if (conf.size() != noutputs())
	   return errh->error("need %d arguments, one per output port", noutputs());
    
    RegexProgram new_program; 

    for (int i=0; i < conf.size(); ++i) {
        String pattern = cp_unquote(conf[i]);
        int result = new_program.add(pattern); 
        if (result < 0) {
            return errh->error("Error (%d) adding pattern %d: %s", result, i, pattern.c_str());
        }
    }

    if (!new_program.compile()) {
        return errh->error("Unable to compile patterns");
    }


    if (!errh->nerrors()) {
        _program.copy(new_program);
        return 0;
    } else {
        return -1;
    }
}

void RegexClassifier::add_handlers() {
    for (uintptr_t i = 0; i != (uintptr_t) noutputs(); ++i) {
    	add_read_handler("pattern" + String(i), read_positional_handler, (void*) i);
    	add_write_handler("pattern" + String(i), reconfigure_positional_handler, (void*) i);
    }
}

void RegexClassifier::push(int, Packet* p) {
    checked_output_push(_program.match(p), p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(RegexClassifier)
ELEMENT_REQUIRES(userlevel)
ELEMENT_MT_SAFE(RegexClassifier)
ELEMENT_LIBS((-L/usr/local/lib -lre2))