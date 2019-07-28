// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_COMMAND_PARSER_H_
#define VIGILANTE_COMMAND_PARSER_H_

#include <string>

namespace vigilante {

class CommandParser {
 public:
  CommandParser() = default;
  virtual ~CommandParser() = default;

  void parse(const std::string& cmd) const;
};

} // namespace vigilante

#endif // VIGILANTE_COMMAND_PARSER_H_
