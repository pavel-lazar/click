#ifndef CLICK_REGEXCLASSIFIER_HH
#define CLICK_REGEXCLASSIFIER_HH
#include <click/element.hh>
#include <re2/re2.h>
#include <re2/set.h>
CLICK_DECLS

/*
=c
RegexClassifier(PATTERN_1, ..., PATTERN_N)

=s local
classifies packets by contents

=d

Classifies packets according to Regex patterns. 
The RegexClassifier has N outputs, each associated with the corresponding pattern
from the configuration string. The pattern match will be done based on all the Packet's 
content. 

You should assume that the patterns are scanned in order, 
and the packet is sent to the output corresponding to the first matching pattern. 
Thus more specific patterns should come before less specific ones. 
If no match is found the packet is discarded.

=n

Patterns are standard Regex syntex as implemented by Google's re2 package. 
For the full supported syntex see:
https://github.com/google/re2/wiki/Syntax 

It's better to enclose each pattern with a pair of "

=e


For example,

  RegexClassifier("xnet",
                  "he.*o",
                  ".*");

creates an element with three outputs. 
The first output will match a packet with the word xnet anywhere in it's content.
The second will match any packet which has a substring starting with he and endind with o.
The third pattern will match everything. 

=h pattern0 rw
Returns or sets the element's pattern 0. There are as many C<pattern>
handlers as there are output ports.

=a Classifier, IPFilter, CheckIPHeader, MarkIPHeader, CheckIPHeader2,
tcpdump(1) */
class RegexClassifier : public Element { 
  public:

    RegexClassifier() CLICK_COLD;
    ~RegexClassifier() CLICK_COLD;

    const char *class_name() const		{ return "RegexClassifier"; }
    const char *port_count() const    { return "1/-"; }
    const char *processing() const    { return PUSH; }
    // this element does not need AlignmentInfo; override Classifier's "A" flag
    const char *flags() const     { return ""; }
    bool can_live_reconfigure() const   { return true; }

    int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;
    void add_handlers() CLICK_COLD;
    void push(int port, Packet *);

  protected:
    class RegexProgram {
      public:
        RegexProgram();
        // RegexProgram(const RegexProgram&);
        //RegexProgram& operator=(const RegexProgram&);
        void copy(const RegexProgram& other);
        ~RegexProgram();
        int add(const String & pattern);
        bool compile(); 
        int match(const Packet *p);
      private:
        bool _compiled;
        Vector<String> _patterns; 
        re2::RE2::Set *_compiled_regex;
    };

    RegexProgram _program;
};

// Match a packet against a set of Regex patterns,
// return the first pattern matched or -1 if no match
inline int RegexClassifier::RegexProgram::match(const Packet* p) {
  std::vector<int> matched_patterns;
  re2::StringPiece data((char*)p->data(), p->length());
  if (!_compiled_regex->Match(data, &matched_patterns)) {
    return -1; 
  }

  int first_match = matched_patterns[0];
  for (unsigned i=1; i < matched_patterns.size(); ++i) {
    if (matched_patterns[i] < first_match) {
      first_match = matched_patterns[i];
    }
  }

  return first_match;
}

inline int RegexClassifier::RegexProgram::add(const String& pattern) {
  if (!_compiled_regex) {
    return -2; 
  }
  int result = _compiled_regex->Add(re2::StringPiece(pattern.c_str(), pattern.length()), NULL);
  if (result >= 0) {
    _patterns.push_back(pattern);  
  } 

  return result;
}

inline bool RegexClassifier::RegexProgram::compile() {
  if (!_compiled_regex) {
    return false;
  }

  _compiled = true;
  return _compiled_regex->Compile();
}

CLICK_ENDDECLS
#endif
