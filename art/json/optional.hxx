#ifndef art__json__optional_hxx_
#define art__json__optional_hxx_

/*
 * This file will be REMOVED once <optional> is widely available.
 */

#if !defined(__has_include)
#  error "__has_include not supported"
#endif

#if __has_include(<optional>)
#  include <optional>
#elif __has_include(<experimental/optional>)
#  include <experimental/optional>
#else
#  error "No support for <optional>"
#endif

namespace art::json {

#if __has_include(<optional>)
using std::optional;
#elif __has_include(<experimental/optional>)
using std::experimental::optional;
#endif

} // namespace art::json

#endif
