/**
 * @file FileBase.h
 * @brief 文件基础类
 * @details 文件基础类，制定了文件相关操作的接口，包括Open、New、Save
 * @author WT
 * @date 2019-07-26
 */
#pragma once

#include "Error.h"
#include "FileSystem.h"
#include "TString.h"
#include "Serialize.h"

#include <memory>

namespace ytlib {
/**
 * @brief 文件基础类
 * 制定了文件相关操作的接口，包括Open、New、Save。默认为txt后缀
 */
template <class T>
class FileBase {
 protected:
  tstring m_filepath;
  bool m_bInitialized;

  ///检查文件名称正确性。如果子类需要检查文件名则重载此函数
  virtual bool CheckFileName(const tstring& filename) const {
    //一般就是检查后缀名
    /*
      tstring Suffix = T_TEXT("txt");
      if (ToLower(filename.substr(filename.length() - Suffix.length(), Suffix.length())) != Suffix) {
        return false;
      }
   */
    return true;
  }
  ///新建一个文件内容结构体。做一些初始化的工作
  virtual bool CreateFileObj() {
    m_fileobj = std::make_shared<T>();
    return true;
  }
  ///从打开的文件中解析获取文件内容结构体
  virtual bool GetFileObj() = 0;
  ///将当前的文件内容结构体保存为文件
  virtual bool SaveFileObj() = 0;

 public:
  FileBase() : m_bInitialized(false), m_fileobj(std::make_shared<T>()) {}
  virtual ~FileBase() {}

  ///文件内容解析后的结构体。提供直接访问
  std::shared_ptr<T> m_fileobj;

  ///子类在open成功后解析文件获得结构体，最终成功才返回success
  void OpenFile(const tstring& path) {
    //检查文件名称与路径
    if (!CheckFileName(path)) {
      throw ytException(Error::ER_ERROR_INVALID_FILENAME);
    }
    tpath filepath = tGetAbsolutePath(path);
    m_filepath = filepath.string<tstring>();
    if (!boost::filesystem::exists(filepath)) {
      throw ytException(Error::ER_ERROR_FILE_NOT_EXIST);
    }
    if (!CreateFileObj()) {
      throw ytException(Error::ER_ERROR_NEW_FILE_FAILED);
    }
    if (!GetFileObj()) {
      throw ytException(Error::ER_ERROR_PARSE_FILE_FAILED);
    }
    m_bInitialized = true;
  }

  ///子类在新建结构体成功后才返回success
  void NewFile() {
    m_filepath.clear();
    if (!CreateFileObj()) {
      throw ytException(Error::ER_ERROR_NEW_FILE_FAILED);
    }
    m_bInitialized = true;
  }
  ///新建结构体并保存到path下
  void NewFile(const tstring& path) {
    //检查文件名称与路径
    if (!CheckFileName(path)) {
      throw ytException(Error::ER_ERROR_INVALID_FILENAME);
    }
    tpath filepath = tGetAbsolutePath(path);
    m_filepath = filepath.string<tstring>();
    if (!CreateFileObj()) {
      throw ytException(Error::ER_ERROR_NEW_FILE_FAILED);
    }
    m_bInitialized = true;
    return (SaveFile());
  }
  ///子类在保存成功，将结构体写成文件后才返回success
  void SaveFile() {
    if ((m_filepath.empty()) || (!m_bInitialized)) {
      throw ytException(Error::ER_ERROR_INVALID_SAVE);
    }
    tpath filepath = tGetAbsolutePath(m_filepath).parent_path();
    if ((!boost::filesystem::exists(filepath)) && (!boost::filesystem::create_directories(filepath))) {
      throw ytException(Error::ER_ERROR_INVALID_SAVE);
    }
    if (!SaveFileObj()) {
      throw ytException(Error::ER_ERROR_SAVE_FILE_FAILED);
    }
  }

  ///子类在另存为成功，将结构体写成文件后才返回success
  void SaveFile(const tstring& path) {
    if (!m_bInitialized) {
      throw ytException(Error::ER_ERROR_INVALID_SAVE);
    }
    //检查文件名称与路径
    if (!CheckFileName(path)) {
      throw ytException(Error::ER_ERROR_INVALID_FILENAME);
    }
    tpath filepath = tGetAbsolutePath(path);
    m_filepath = filepath.string<tstring>();
    filepath = filepath.parent_path();
    if ((!boost::filesystem::exists(filepath)) && (!boost::filesystem::create_directories(filepath))) {
      throw ytException(Error::ER_ERROR_INVALID_SAVE);
    }
    if (!SaveFileObj()) {
      throw ytException(Error::ER_ERROR_SAVE_FILE_FAILED);
    }
  }

  inline bool isInitialized() const {
    return m_bInitialized;
  }

  inline tstring GetFilePath() const {
    return m_filepath;
  }
  ///获取去掉文件名后缀的名称
  tstring GetFileName() const {
    if (m_filepath.empty()) return T_TEXT("");
    tpath filepath = tGetAbsolutePath(m_filepath);
    tstring fname = filepath.filename().string<tstring>();
    std::size_t pos = fname.find(T_TEXT('.'));
    if (pos < fname.length()) {
      fname = fname.substr(0, pos);
    }
    return fname;
  }
  ///获取上级目录
  tstring GetFileParentPath() const {
    if (m_filepath.empty()) return T_TEXT("");
    tpath filepath = tGetAbsolutePath(m_filepath);
    return filepath.parent_path().string<tstring>();
  }
};
}  // namespace ytlib