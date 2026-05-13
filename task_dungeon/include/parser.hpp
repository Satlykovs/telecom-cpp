#pragma once
#include "models.hpp"

#include <stdexcept>
#include <string>

class ParseError : public std::runtime_error
{
  public:
    explicit ParseError(const std::string& line) : std::runtime_error(line) {}
};

Config parseInput(const std::string& filename);