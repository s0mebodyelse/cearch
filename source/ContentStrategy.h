#ifndef _H_CONTENTSTRATEGY
#define _H_CONTENTSTRATEGY

#include <string>

template <typename T>
class ContentStrategy {
   public:
    virtual ~ContentStrategy() = default;
    virtual std::string read_content(T const&) const = 0;
};

#endif