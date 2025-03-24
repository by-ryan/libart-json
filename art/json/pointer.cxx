#include <art/json/pointer.hxx>

namespace art::json {

pointer::
pointer(std::string const& expression)
  : refs_{parse_refs(expression)}
{}

optional< variant >
pointer::
read(variant const& source) const
{
  auto const* current = &source;
  auto refs = refs_;

  while (refs.size() > 0) {
    auto cref = refs.front();
    refs.pop();

    if (current->is_object()) {
      if (!current->contains(cref))
        return std::nullopt;

      current = &current->get(cref);
    }
    else if (current->is_array()) {
      if (cref.size() == 1 && '~' == cref[0])
        throw invalid_array_index{ cref };

      auto i = std::stoul(cref);

      if (current->size() <= i)
        throw invalid_array_index{ cref };

      current = &current->get(i);
    }
    else {
      throw unexpected_type{};
    }
  }

  return *current;
}

void
pointer::
write(variant& target, variant value) const
{
  if (!target.is_object())
    throw std::invalid_argument{"expected object"};

  auto* current = &target;
  auto refs = refs_;

  while (refs.size() > 1) {
    auto cref = refs.front();
    refs.pop();

    // Check what current is.
    if (current->is_object()) {
      if (current->contains(cref)) {
        current = &current->get(cref);
      }
      else {
        current->set(cref, std::map< std::string, variant >{});
        current = &current->get(cref);
      }
    }
    else if (current->is_array()) {
      if (cref.size() == 1 && '~' == cref[0])
        throw invalid_array_index{ cref };

      auto i = std::stoul(cref);

      if (current->size() <= i)
        throw invalid_array_index{ cref };

      current = &current->get(i);
    }
    else {
      throw unexpected_type{};
    }
  }

  if (refs.empty())
    throw std::invalid_argument{"invalid argument"};

  auto cref = refs.front();
  refs.pop();

  if (current->is_object()) {
    current->set(cref, value);
  }
  else if (current->is_array()) {
    // TODO: Implement.
  }
  else {
    throw unexpected_type{};
  }
}

std::queue< std::string >
pointer::
parse_refs(std::string const& expression)
{
  std::queue< std::string > refs;

  for (auto it = std::begin(expression); it != std::end(expression);) {
    if ('/' != *it)
      throw invalid_syntax{};

    ++it;

    std::string cref;

    while (it != std::end(expression) && '/' != *it) {
      switch (*it) {
        case '~':
          ++it; // skip tilde
          if (it == std::end(expression))
            throw invalid_syntax{};
          else if ('0' == *it)
            cref += '~';
          else if ('1' == *it)
            cref += '/';
          else
            throw invalid_syntax{};
          ++it;
          break;
        default:
          cref += *it;
          ++it;
          break;
      }
    }

    if (!cref.empty())
      refs.push(std::move(cref));
  }

  return refs;
}

std::string
to_string(pointer const& ptr)
{
  std::string rendered;

  auto refs = ptr.refs_;

  while (refs.size() > 0) {
    auto cref = refs.front();
    refs.pop();

    rendered += '/';
    rendered += cref;
  }

  return rendered;
}

} // namespace art::json
