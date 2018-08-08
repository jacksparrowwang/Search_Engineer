#pragma once

#include <vector>
#include <unordered_map>
#include "index.pb.h"

namespace doc_index {

// 定义一些类型
typedef doc_index_proto::DocInfo DocInfo;
typedef doc_index_proto::Weight Weight;
typedef std::vector<DocInfo> ForwardIndex;
typedef std::vector<Weight> InvertedList;  // 倒排拉链
typedef std::unordered_map<std::string, InvertedList> InvertedIndex;

// 索引模块核心类. 和索引相关的全部操作都包含在这个类中
// a) 构建, raw_input 中的内容进行解析在内存中构造
//    出索引结构(hash)
// b) 保存, 把内存中的索引结构进行序列化, 存到文件中
//    (序列化就依赖了刚才的 index.proto) 制作索引的可执行
//    程序来调用保存
// c) 加载, 把磁盘上的索引文件读取出来, 反序列化, 生成
//    内存中的索引结构 搜索服务器
// d) 反解, 内存中的索引结果按照一定的格式打印出来, 方便
//    测试
// e) 查正排, 给定文档id, 获取到文档的详细信息
// f) 查倒排, 给定关键词, 获取到和关键词相关的文档列表
class Index {
public:
  Index();

  static Index* Instance() {
    if (inst_ == NULL) {
      inst_ = new Index();
    }
    return inst_;
  }

  // 从 raw_input 文件中读数据, 在内存中构建成索引结构
  bool Build(const std::string& input_path);

  // 把内存中的索引数据保存到磁盘上
  bool Save(const std::string& output_path);

  // 把磁盘上的文件加载到内存的索引结构中
  bool Load(const std::string& index_path);

  // 调试用的接口, 把内存中的索引数据按照一定的格式打印到
  // 文件中
  bool Dump(const std::string& forward_dump_path,
            const std::string& inverted_dump_path);

  // 根据 doc_id 获取到 文档详细信息
  const DocInfo* GetDocInfo(uint64_t doc_id) const;

  // 根据关键词获取到 倒排拉链(包含了一组doc_id)
  const InvertedList* GetInvertedList(const std::string& key) const;
  
private:
  ForwardIndex forward_index_;
  InvertedIndex inverted_index_;

  static Index* inst_;
};

}  // end doc_index
