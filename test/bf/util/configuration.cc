#include "util/configuration.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <fstream>

namespace util {

config_error::config_error(std::string msg)
  : std::logic_error(std::move(msg))
{
}

config_error::config_error(std::string msg, char shortcut)
  : std::logic_error(msg + " (-" + shortcut + ")")
{
}

config_error::config_error(std::string msg, std::string option)
  : std::logic_error(msg + " (--" + option + ")")
{
}

// TODO: Implement this function.
void configuration::load(std::string const& filename)
{
  std::ifstream ifs(filename);
  if (! ifs)
    throw config_error("could not open configuration file");

  verify();
}

void configuration::load(int argc, char *argv[])
{
  for (int i = 1; i < argc; ++i)
  {
    std::vector<std::string> values;
    std::string arg(argv[i]);
    if (arg.size() < 2)
    {
      throw config_error("ill-formed option specificiation", argv[i]);
    }
    else if (arg[0] == '-' && arg[1] == '-')
    {
      // Argument must begin with '--'.
      if (arg.size() == 2)
        throw config_error("ill-formed option specification", argv[i]);
      arg = arg.substr(2);
    }
    else if (arg[0] == '-')
    {
      if (arg.size() > 2)
        // The short option comes with a value.
        values.push_back(arg.substr(2));
      arg = arg[1];
      auto s = shortcuts_.find(arg);
      if (s == shortcuts_.end())
        throw config_error("unknown short option", arg[0]);
      arg = s->second;
    }

    auto o = find_option(arg);
    if (! o)
      throw config_error("unknown option", arg);
    o->defaulted_ = false;

    while (i + 1 < argc && std::strlen(argv[i+1]) > 0 && argv[i+1][0] != '-')
      values.emplace_back(argv[++i]);
    if (values.size() > o->max_vals_)
      throw config_error("too many values", arg);
    if (o->max_vals_ == 1 && values.size() != 1)
      throw config_error("option value required", arg);
    if (! values.empty())
      o->values_ = std::move(values);
  }
  verify();
}

bool configuration::check(std::string const& opt) const
{
  auto o = find_option(opt);
  return o && ! o->defaulted_;
}

std::string const& configuration::get(char const* opt) const
{
  auto o = find_option(opt);
  if (! o)
    throw config_error("invalid option cast", opt);
  if (o->values().empty())
    throw config_error("option has no value", opt);
  if (o->max_vals_ > 1)
    throw config_error("cannot get multi-value option", opt);
  return o->values_[0];
}

void configuration::usage(std::ostream& sink, bool show_all)
{
  sink << banner_ << "\n";
  for (auto& b : blocks_)
  {
    if (! show_all && ! b.visible_)
      continue;
    sink << "\n " << b.name_ << ":\n";
    auto has_shortcut = std::any_of(
        b.options_.begin(),
        b.options_.end(),
        [](option const& o) { return o.shortcut_ != '\0'; });
    auto max = std::max_element(
        b.options_.begin(),
        b.options_.end(),
        [](option const& o1, option const& o2)
        {
          return o1.name_.size() < o2.name_.size();
        });
    auto max_len = max->name_.size();
    for (auto& opt : b.options_)
    {
      sink << "   --" << opt.name_;
      sink << std::string(max_len - opt.name_.size(), ' ');
      if (has_shortcut)
        sink << (opt.shortcut_ ? std::string(" | -") + opt.shortcut_ : "     ");
      sink << "   " << opt.description_ << "\n";
    }
  }
  sink << std::endl;
}

configuration::option::option(std::string name,
                              std::string desc, char shortcut)
  : name_(std::move(name)),
    description_(std::move(desc)),
    shortcut_(shortcut)
{
}

configuration::option& configuration::option::multi(size_t n)
{
  max_vals_ = n;
  return *this;
}

configuration::option& configuration::option::single()
{
  return multi(1);
}

std::vector<std::string> const& configuration::option::values() const
{
  return values_;
}

configuration::block::block(block&& other)
  : visible_(other.visible_),
    name_(std::move(other.name_)),
    prefix_(std::move(other.prefix_)),
    options_(std::move(other.options_)),
    config_(other.config_)
{
  other.visible_ = true;
  other.config_ = nullptr;
}

configuration::option&
configuration::block::add(std::string name, std::string desc)
{
  std::string fqn = qualify(name);
  if (config_->find_option(fqn))
    throw config_error("option already exists", std::move(fqn));
  options_.emplace_back(std::move(fqn), std::move(desc));
  return options_.back();
}

configuration::option&
configuration::block::add(char shortcut, std::string name, std::string desc)
{
  if (config_->shortcuts_.count({shortcut}))
    throw config_error("option shortcut already exists", shortcut);
  config_->shortcuts_.insert({{shortcut}, std::move(qualify(name))});
  std::string fqn = qualify(name);
  if (config_->find_option(fqn))
    throw config_error("option already exists", std::move(fqn));
  options_.emplace_back(std::move(fqn), std::move(desc), shortcut);
  return options_.back();
}

configuration::block& configuration::create_block(std::string name,
                                                  std::string prefix)
{
  block b(std::move(name), std::move(prefix), this);
  blocks_.push_back(std::move(b));
  return blocks_.back();
}

bool configuration::block::visible() const
{
  return visible_;
}

void configuration::block::visible(bool flag)
{
  visible_ = flag;
}

configuration::block::block(std::string name,
                            std::string prefix,
                            configuration* config)
  : name_(std::move(name)),
    prefix_(std::move(prefix)),
    config_(config)
{
}

std::string configuration::block::qualify(std::string const& name) const
{
  return ! prefix_.empty() ? prefix_ + separator + name : name;
}

void configuration::conflicts(std::string const& opt1,
                              std::string const& opt2) const
{
  if (check(opt1) && check(opt2))
  {
    std::string msg = "conflicting options";
    msg += " (--" + opt1 + " conflicts with --" + opt2 + ")";
    throw config_error(std::move(msg));
  }
}

void configuration::depends(std::string const& needy,
                            std::string const& required) const
{
  if (check(needy) && ! check(required))
  {
    std::string msg = "missing option dependency";
    msg += " (--" + needy + " needs --" + required + ")";
    throw config_error(std::move(msg));
  }
}

void configuration::banner(std::string banner)
{
  banner_ = std::move(banner);
}

void configuration::verify()
{
  // The default implementation doesn't do anything.
}

configuration::option*
configuration::find_option(std::string const& opt)
{
  for (auto& b : blocks_)
    for (size_t i = 0; i < b.options_.size(); ++i)
      if (b.options_[i].name_ == opt)
        return &b.options_[i];
  return nullptr;
}

configuration::option const*
configuration::find_option(std::string const& opt) const
{
  for (auto& b : blocks_)
    for (size_t i = 0; i < b.options_.size(); ++i)
      if (b.options_[i].name_ == opt)
        return &b.options_[i];
  return nullptr;
}

} // namespace util
