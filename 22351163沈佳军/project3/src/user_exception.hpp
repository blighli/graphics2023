// Created by lonelyinnovator on 2023/2/26 23:08.
/**
 * Define user exception and sub-exceptions.
 */

#ifndef USER_EXCEPTION_HPP
#define USER_EXCEPTION_HPP

#include <iostream>
#include <string>
#include <utility>

#define FMT_HEADER_ONLY

#include <fmt/format.h>

/**
 * User exception, base class.
 * Examples:
 *   try {
 *     ...
 *   }
 *   catch(const UserException &e) {
 *     std::cout << e.what() << std::endl;
 *   }
 */
class UserException : public std::exception {
 public:
  /**
   * User exception constructor.
   * @param str error message
   * @param id exception id, base class is 0
   */
  explicit UserException(const char *str = nullptr, int id = 0) : error_message_(str), id_(id) {};

  /**
   * User exception constructor.
   * @param str error message
   * @param id exception id, base class is 0
   */
  explicit UserException(std::string str = "", int id = 0) : error_message_(std::move(str)), id_(id) {};

  /**
   * Return formatted error message.
   * @return formatted error message
   */
  [[nodiscard]] virtual const char *what() const = 0;

 protected:
  // error message
  std::string error_message_;
  // exception id
  int id_;
  // what function return the error string
  mutable std::string error_string_;
};


/**
 * Filestream exception, sub class of User exception.
 * Examples:
 *   if (error) {
 *     throw FStreamException("error message");
 *   }
 */
class FStreamException : public UserException {
 public:
  /**
   * FileStream exception constructor.
   * @param str error message
   * @param id exception id
   */
  explicit FStreamException(const char *str = nullptr, int id = 1) : UserException(str, id) {};

  /**
   * FileStream exception constructor.
   * @param str error message
   * @param id exception id
   */
  explicit FStreamException(std::string str = "", int id = 1) : UserException(std::move(str), id) {};

  /**
   * Return formatted error message.
   * @return formatted error message
   */
  [[nodiscard]] const char *what() const override {
    if (!error_message_.empty() && id_ >= 0) {
      error_string_ = fmt::format("Fstream error message: {}, id: {}", error_message_, id_);
    } else {
      error_string_ = "Unknown error";
    }
    return error_message_.c_str();
  }
};


#endif //USER_EXCEPTION_HPP
