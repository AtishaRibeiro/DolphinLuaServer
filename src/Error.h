#pragma once

#include <optional>
#include <string>
#include <variant>

class Error {
public:
  Error(std::string message) : mMessage(message) {}
  std::string str() const { return mMessage; }

private:
  std::string mMessage;
};

template <class T> class Expected {
public:
  Expected(T value) : mExpected(value){};
  Expected(Error error) : mExpected(error){};

  bool hasValue() const { return std::holds_alternative<T>(mExpected); };
  bool isError() const { return std::holds_alternative<Error>(mExpected); };

  T getValue() const { return std::get<T>(mExpected); }

  Error getError() const { return std::get<Error>(mExpected); }

  Expected<T> operator>>(Expected<T> (*f)(T)) {
    if (!hasValue()) {
      return this;
    }
    return (*f)(getValue());
  }

private:
  std::variant<T, Error> mExpected;
};

class Result {
public:
  Result() : mError() {}
  Result(Error error) : mError(error) {}

  bool isError() const { return mError.has_value(); }

  Error getError() const { return mError.value(); }

private:
  std::optional<Error> mError;
};

#define RET_ON_ERR(result)                                                     \
  if (result.IsError())                                                        \
    return result.GetError();
