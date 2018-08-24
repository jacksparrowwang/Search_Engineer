#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <boost/algorithm/string.hpp>
#include <sys/time.h>

namespace common {

class StringUtil {
public:
  // 如何理解压缩关闭:
  // 对于这个字符串, 如果关闭压缩, 切分结果返回三个部分
  // 如果打开压缩, 切分结果两个部分, 把两个相邻的合并成了一个
  static void Split(const std::string& input,
                     std::vector<std::string>* output,
                     const std::string& split_char) {
    boost::split(*output, input,
                 boost::is_any_of(split_char),// 切割的字符
                 boost::token_compress_off);
  }

  static int32_t FindSentenceBeg(const std::string& content,
      int32_t first_pos) {
    // 从 first_pos 开始往前找, 找到句子的开始位置
    // 如果是拿 . 作为句子的结束标记, 对于以下情况:
    // a.size() 就不应该作为一个句子的开始
    // hello world. hehe
    // 其实此处的逻辑还可以更加的详细, 考虑到更多的句子分割的情况
    for (int32_t i = first_pos; i >= 0; --i) {
      if (content[i] == ';' || content[i] == ','
          || content[i] == '?' || content[i] == '!' 
          || (content[i] == '.' && content[i + 1] == ' ')) {
        return i + 1;
      }
    }
    // 如果往前找的过程中没有找到句子的分割符, 就认为当前所在的句子
    // 就是文章的第一句话. 也就可以用 0 表示句子的开始
    return 0;
  }
};

class DictUtil {
public:
  // 从文件中加载暂停词的内容
  bool Load(const std::string& path) {
    std::ifstream file(path.c_str());
    if (!file.is_open()) {
      // 此处并不进行 LOG(FATAL) , 主要因为当前这个类
      // 可能会在很多不同的场景下被使用到. 所以错误级别
      // 交给调用者来决定
      return false;
    }
    std::string line;
    while (std::getline(file, line)) {
      set_.insert(line);
    }
    return true;
  }

  bool Find(const std::string& key) const {
    return set_.find(key) != set_.end();
  }
private:
  std::unordered_set<std::string> set_;
};

class FileUtil {
public:
  // 把文件的所有内容都读到 content 中
  static bool Read(const std::string& input_path, 
        std::string* content) {
    std::ifstream file(input_path.c_str());
    if (!file.is_open()) {
      return false;
    }
    // 需要先获取到文件的长度
    // 把文件光标放到文件末尾
    file.seekg(0, file.end);
    // 获取到文件光标的位置
    int length = file.tellg();
    file.seekg(0, file.beg);
    content->resize(length);
    file.read(const_cast<char*>(content->data()), length);
    file.close();
    return true;
  }

  // 把 content 中的所有内容都写到文件中
  static bool Write(const std::string& output_path,
        const std::string& content) {
    std::ofstream file(output_path.c_str());
    if (!file.is_open()) {
      return false;
    }
    file.write(content.data(), content.size());
    file.close();
    return true;
  }
};

class TimeUtil {
public:
  // 获取到秒级时间戳
  static int64_t TimeStamp() {
    struct timeval tv;
    ::gettimeofday(&tv, NULL);
    return tv.tv_sec;
  }

  // 获取到毫秒级时间戳
  static int64_t TimeStampMS() {
    struct timeval tv;
    ::gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
  }

  // 微秒级时间戳
  static int64_t TimeStampUS() {
    struct timeval tv;
    ::gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 * 1000 + tv.tv_usec;
  }
};
}  // end common
