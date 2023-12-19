// Created by lonelyinnovator on 2023/2/26 16:47.

#include "file_process.h"

FileProcess::FileProcess(const std::string &file_path, int mode) {
  OpenFile(file_path, mode);
}

FileProcess::~FileProcess() {
  CloseFile();
}

void FileProcess::OpenFile(const std::string &file_path, int mode) {
  CloseFile();
//  fstream_.exceptions(std::fstream::badbit);
  fstream_.open(file_path, mode);
  if (!fstream_.is_open()) {
    throw FStreamException(fmt::format("fstream open {} error!", file_path));
  }
}

void FileProcess::CloseFile() {
  if (fstream_.is_open()) {
    fstream_.close();
  }
}

std::string FileProcess::ReadLine() {
  std::string res;
  std::getline(fstream_, res);
  return res;
}

std::string FileProcess::ReadAll() {
  std::stringstream string_stream;
  string_stream << fstream_.rdbuf();
  return string_stream.str();
}

bool FileProcess::IsEOF() {
  return fstream_.eof();
}

void FileProcess::Write(const std::string &str) {
  fstream_ << str;
}

void FileProcess::Write(const char *str) {
  fstream_ << str;
  std::ifstream f;
}

void FileProcess::Write(const char *str, const int size) {
  fstream_.write(str, size);
}

std::fstream &FileProcess::GetFstream() {
  return fstream_;
}










