// Created by lonelyinnovator on 2023/2/26 16:47.
/**
 * Process file, read and write.
 */

#ifndef FILE_PROCESS_H
#define FILE_PROCESS_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define FMT_HEADER_ONLY

#include <fmt/core.h>

#include "user_exception.hpp"


/**
 * Read or write file.
 * Examples:
 *   // read:
 *   FileProcess file_process(file_path);
 *   auto res = file_process.ReadAll();
 *   // write:
 *   while (!file_process.IsEOF()) {
 *     file_process.Write("something");
 *   };
 */
class FileProcess {
 public:
  /**
   * Init FileProcess with filepath and mode.
   * @param file_path file path
   * @param mode read/write mode, see std::ios
   */
  explicit FileProcess(const std::string &file_path, int mode = std::ios::in | std::ios::out);

  /**
   * Deconstruct, close file.
   */
  ~FileProcess();

  /**
   * Open file with filepath and mode.
   * @param file_path file path
   * @param mode read/write mode, see std::ios
   */
  void OpenFile(const std::string &file_path, int mode = std::ios::in | std::ios::out) noexcept(false);

  /**
   * Close file.
   */
  void CloseFile();

  /**
   * Read all from opened file.
   * @return file content string
   */
  std::string ReadAll();

  /**
   * Read one line from opened file.
   * @return one line string
   */
  std::string ReadLine();

  /**
   * If is read end of file.
   * @return true if end of file; false not
   */
  bool IsEOF();

  /**
   * Write string to opened file.
   * @param str string to write
   */
  void Write(const std::string &str);

  /**
 * Write string to opened file.
 * @param str string to write
 */
  void Write(const char *str);

  /**
   * Write string within size to opened file.
   * @param str string to write
   * @param size size of string to write
   */
  void Write(const char *str, int size);

  /**
   * Get fstream.
   * @return fstream
   */
  std::fstream &GetFstream();

 private:
  // fstream to read/write
  std::fstream fstream_;
};


#endif //FILE_PROCESS_H
