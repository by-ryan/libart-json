namespace art::json {

inline emitter::emitter(std::ostream& output) : output_{ output }
{}

inline std::ostream&
emitter::output()
{
  return output_;
}

inline void
emitter::operator()(variant::undefined_t const& value)
{
  output() << "null";
}

inline void
emitter::operator()(bool const& value)
{
  output() << (value ? "true" : "false");
}

inline void
emitter::operator()(long long int const& value)
{
  output() << std::to_string(value);
}

inline void
emitter::operator()(unsigned long long int const& value)
{
  output() << std::to_string(value);
}

inline void
emitter::operator()(long double const& value)
{
  output() << std::to_string(value);
}

inline void
emitter::operator()(std::string const& value)
{
  output() << '\"';

  for (auto const j : value) {
    switch (j) {
      case '\n':
        output() << "\\n";
        break;
      case '\t':
        output() << "\\t";
        break;
      case '\\':
        output() << "\\";
        break;
      case '"':
        output() << "\\\"";
        break;
        // case '\x22':
        // case '\x5c':
        // case '\x2f':
        // case '\x62':
        // case '\x66':
        // case '\x6e':
        // case '\x72':
        // case '\x74':
      default:
        output() << j;
    }
  }

  output() << '\"';
}

inline void
emitter::operator()(std::vector< variant > const& value)
{
  if (value.empty()) {
    output() << "[]";
    return;
  }

  output() << '[' << '\n';

  ++indent_;

  bool first{ true };
  for (auto const& j : value) {
    if (!first)
      output() << ',' << '\n';
    do_indent();
    first = false;
    visit(*this, j.value_);
  }
  output() << '\n';
  --indent_;

  do_indent();
  output() << ']';
}

inline void
emitter::operator()(std::map< std::string, variant > const& value)
{
  if (value.empty()) {
    output() << "{}";
    return;
  }

  output() << "{\n";

  ++indent_;

  bool first{ true };
  for (auto const& j : value) {
    if (!first)
      output() << ",\n";
    first = false;
    do_indent();
    (*this)(j.first);
    output() << ':' << ' ';
    visit(*this, j.second.value_);
  }
  output() << '\n';
  --indent_;

  do_indent();
  output() << '}';
}

inline void
emitter::do_indent()
{
  std::size_t i{ indent_ * 2 };
  while (i-- > 0)
    output() << ' ';
}

} // namespace art::json
