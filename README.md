```markdown
# MySTL - 自定义 C++ 标准库部分实现

![GitHub license](https://img.shields.io/github/license/TMac-wei/mystl.svg)
![GitHub stars](https://img.shields.io/github/stars/TMac-wei/mystl.svg)

MySTL是我针对常见的部分c++标准库的学习和复现，包括容器、部分算法、迭代器等核心组件的自定义实现，部分内容采用了最新的c++特性。

## 功能特性
- 实现了 vector、list、map 等常用容器
- 提供多种算法（排序、查找、遍历等）
- 支持 C++11 及以上标准
- 详细的注释和测试用例

## 安装
```bash
git clone https://github.com/TMac-wei/mystl.git
cd mystl
mkdir build && cd build
cmake ..
make
