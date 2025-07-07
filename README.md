# mySTL - 自制 C++ STL 学习实现  

## 项目简介  
这是一个**用于学习 C++ STL 底层原理**的自制库，通过手动实现 STL 部分核心组件（容器、迭代器、算法等），帮助理解 C++ 标准库的设计逻辑与实现细节。  

适合 C++ 进阶学习者，从代码层面剖析：  
- 容器（如 `vector`/`string`/`stack`/`queue`/`priority_queue`/`unordered_set`/`unordered_map` 等）的内存管理与操作原理  
- 迭代器的分类、适配与 traits 技术  
- 算法（如排序、查找）的泛型实现  
- 内存分配、类型萃取等基础组件
- 复现了智能指针部分内容如（`shared_ptr`/`weak_ptr`/`unique_ptr`）

## 功能概览  
### 已实现组件  
| 模块目录       | 核心内容                                                                 |  
|----------------|--------------------------------------------------------------------------|  
| `chrono_`      | 简单时间相关工具（`duration`/`ratio` 基础实现）                          |  
| `iterator_`    | 各类迭代器（插入迭代器、反向迭代器、流迭代器等）及迭代器辅助工具（`advance`/`distance`） |  
| `memory_`      | 内存分配与管理相关（需结合代码确认具体实现，如 allocator 基础逻辑）       |  
| `mystl_type_traits` | 类型萃取工具（判断类型属性、提取类型特征）                               |  
| `test`         | 测试用例（验证各组件功能正确性，该文件夹的内容仅仅是我自己在完成头文件编写之后的测试用例，若需补充用例时可参考）                     |  
| `utility_`     | 辅助工具（如 `pair`/`optional` 等基础结构）                              |  
| 根目录文件     | 容器（`vector`/`list`/`map` 等）、智能指针（`shared_ptr`）、字符串（`string`）等核心实现 |  


### 典型实现亮点  
- **迭代器体系**：通过 `iterator_traits` 统一迭代器接口，实现插入/反向/流迭代器的适配  
- **容器多样化**：覆盖顺序容器（`vector`/`deque`）、关联容器（`map`/`multiset`）、适配器（`stack`/`queue`）  
- **泛型算法**：基于迭代器的通用算法（排序、查找、遍历），适配自定义容器  
- **内存管理**：简单模拟分配器逻辑（需结合 `memory_` 代码确认细节）  


## 快速上手  
### 环境依赖  
- 编译器：支持 **C++11 及以上** 的 `g++`/`clang`/`MSVC`（推荐 `g++ 7.0+`）  
- 构建工具：`CMake`（用于生成项目构建文件）  


### 编译运行（以 Linux/Mac 为例）  
1. **克隆仓库**：  
   ```bash  
   git clone git@github.com:TMac-wei/mystl.git  
   cd mystl  
   ```  

2. **生成构建文件**：  
   ```bash  
   mkdir build && cd build  
   cmake ..  
   ```  

3. **编译项目**：  
   ```bash  
   make  
   ```  

4. **运行测试**（需补充测试用例到 `test` 目录，或直接测试示例代码）：  
   ```bash  
   # 假设 test 目录有 main.cpp，编译后执行  
   ./test/mystl_test  
   ```  


### 代码示例  
#### 1. 用自定义 `vector` 容器  
```cpp  
#include "my_vector.h"  
#include <iostream>  

int main() {  
    mystl::vector<int> vec;  
    for (int i = 0; i < 5; ++i) {  
        vec.push_back(i);  
    }  

    // 遍历输出：0 1 2 3 4  
    for (auto it = vec.begin(); it != vec.end(); ++it) {  
        std::cout << *it << " ";  
    }  
    return 0;  
}  
```  

#### 2. 反向迭代器用法  
```cpp  
#include "my_list.h"  
#include "my_reverse_iterator.h"  
#include <iostream>  

int main() {  
    mystl::list<int> my_list = {1, 2, 3, 4};  

    // 反向遍历：4 3 2 1  
    for (auto it = my_list.rbegin(); it != my_list.rend(); ++it) {  
        std::cout << *it << " ";  
    }  
    return 0;  
}  
```  


## 目录结构说明  
```  
mySTL/  
├── chrono_/          # 时间相关工具（duration、ratio 等）  
├── cmake-build-debug/ # CMake 构建临时目录（已忽略，无需关注）  
├── file/             # 文件操作相关（需结合代码确认具体内容）  
├── iterator_/        # 迭代器核心实现（插入迭代器、反向迭代器等）  
├── memory_/          # 内存管理（分配器、内存工具）  
├── mystl_type_traits/ # 类型萃取工具（判断类型属性、提取特征）  
├── test/             # 测试用例（需补充完善，验证组件正确性）  
├── utility_/         # 辅助工具（pair、optional 等）  
├── CMakeLists.txt    # CMake 构建配置文件  
├── main.cpp          # 示例入口（可编写简单测试）  
├── my_algorithm.h    # 泛型算法（排序、查找等）  
├── my_array.h        # 数组容器实现  
├── ...（其他容器/算法头文件）...  
```  


## 学习与贡献  
### 学习路径建议  
1. **从基础组件开始**：先看 `utility_`（如 `pair`/`optional`）、`mystl_type_traits`（类型萃取）  
2. **理解迭代器**：`iterator_` 目录的迭代器实现 + `my_iterator.h`，掌握 traits 技术  
3. **容器实现**：`my_vector.h`/`my_list.h`（顺序容器）→ `my_map.h`/`my_set.h`（关联容器）  
4. **算法与适配器**：`my_algorithm.h`（排序/查找） + `my_stack.h`/`my_queue.h`（适配器）  


### 贡献代码  
如果你想参与完善：  
1. **提 Issue**：反馈 Bug、建议功能或讨论实现细节  
2. **提交 PR**：  
   - Fork 仓库 → 新建分支（如 `feature/xxx`）→ 提交代码 → 发起 Pull Request  
   - 遵循现有代码风格（注释清晰、命名与 STL 类似）  


## 许可证  
本项目基于 **MIT 许可证** 开源，可自由用于学习、修改与分发。  


## 联系与交流  
- 仓库地址：[https://github.com/TMac-wei/mystl](https://github.com/TMac-wei/mystl)  
- 遇到问题？直接提 Issue

**学习 STL 底层，从手写开始！** 🚀 欢迎 Star 支持，一起进步～
