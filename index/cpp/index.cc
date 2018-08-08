#include <fstream>
#include <base/base.h>
#include "index.h"

namespace doc_index {

Index* Index::inst_ = NULL;

// 从 raw_input 文件中读数据, 在内存中构建成索引结构
bool Index::Build(const std::string& input_path) {
  LOG(INFO) << "Index Build";
  // 1. 按行读取文件内容, 针对读到的每一行数据进行处理
  std::ifstream file(input_path.c_str());
  CHECK(file.is_open()) << "input_path: " << input_path;
  std::string line;
  while (std::getline(file, line)) {
    // 2. 把这一行数据制作成一个 DocInfo
    //    此处获取到的 doc_info 是为了接下来制作倒排方便
    const DocInfo* doc_info = BuildForward(line);
    // 如果构建正排失败, 就立刻让进程终止
    CHECK(doc_info == NULL);
    // 3. 更新倒排信息
    // 此函数的输出结果, 直接放到 Index::inverted_index_
    BuildInverted(*doc_info);
  }
  // 4. 处理完所有的文档之后, 针对所有的倒排拉链进行排序
  //    key-value 中的value进行排序. 排序的依据按照权重
  //    降序排序
  SortInverted();
  file.close();
  LOG(INFO) << "Index Build Done!!!";
  return true;
}

// 把内存中的索引数据保存到磁盘上
bool Index::Save(const std::string& output_path) {
  return true;
}

// 把磁盘上的文件加载到内存的索引结构中
bool Index::Load(const std::string& index_path) {
  return true;
}

// 调试用的接口, 把内存中的索引数据按照一定的格式打印到
// 文件中
bool Index::Dump(const std::string& forward_dump_path,
    const std::string& inverted_dump_path) {
  return true;
}

// 根据 doc_id 获取到 文档详细信息
const DocInfo* Index::GetDocInfo(uint64_t doc_id) const {
  return NULL;
}

// 根据关键词获取到 倒排拉链(包含了一组doc_id)
const InvertedList* Index::GetInvertedList(
                const std::string& key) const {
  return NULL;
}
}  // end doc_index
