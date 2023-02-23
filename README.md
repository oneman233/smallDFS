# smallDFS

[Reference](https://github.com/fangpin/miniDFS/)

#### 轻型分布式文件系统

* 基于 C++ 实现的分布式文件系统，将文件切分成 2MB 大小的 chunk，支持用户指定 chunk 备份数
* 实现了命名服务器储存的文件路径树，并利用条件变量实现了多线程数据服务器
* 实现了三种不同的负载均衡策略：最小容量优先、随机均分、顺序轮询，支持 MD5 校验完整性

#### 技术原理

* 给每个 dataserver 分配等长 buffer，再根据 offset 在 buffer 中的不同位置写入数据
  * offset 可以理解为源文件被切分成的 chunk 的序列号
  * 读写数据时才分配 buffer，读写完成后即销毁
* 文件树储存 parent 和 next sibling 指针，查询时采取层序遍历
* 通过条件变量将 dataserver 挂起等待，再通过 finish 指针告知 nameserver 任务执行情况
* 负载均衡策略及 MD5 实现位于 `utils.cpp`