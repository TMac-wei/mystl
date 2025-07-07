/**
 * @file      my_exception.h
 * @brief     [异常处理]
 * @author    Weijh
 * @date      2025/7/3
 * @version   1.0
 */

#pragma once

/// 关于异常的复现,支持深拷贝和内存管理
#include <cstring>
#include <cstdlib>

namespace my {

/// 异常数据结构体
    struct ExceptionData {
        const char *what;       /// 错误信息
        bool do_free;
    };

    class exception {
    public:
        /// 默认构造：无异常信息
        exception() noexcept: data_{nullptr, false} {}

        /// 接收 C 风格字符串构造异常对象，标记 do_free = true，但目前没有真正释放内存
        explicit exception(const char *what) noexcept {
            if (what) {
                size_t len = std::strlen(what);
                char *copy = static_cast<char *>(std::malloc(len));
                if (copy) {
                    std::strcpy(copy, what);    /// 将错误信息拷贝到copy
                    data_.what = copy;
                    data_.do_free = true;
                } else {
                    /// 分配char* copy失败
                    data_ = {nullptr, false};
                }
            } else {
                data_ = {nullptr, false};
            }
        }

        /// 支持深拷贝
        exception(const exception &other) noexcept {
            if (other.data_.what) {
                size_t len = std::strlen(other.data_.what) + 1;     /// 有最后的"\0"
                char *copy = static_cast<char *>(std::malloc(len));
                if (copy) {
                    std::strcpy(copy, other.data_.what);
                    data_.what = copy;
                    data_.do_free = true;
                } else {
                    /// 分配char* copy失败
                    data_ = {nullptr, false};
                }
            } else {
                data_ = {nullptr, false};
            }
        }

        exception &operator=(const exception &other) noexcept {
            /// 自赋值检查
            if (this != &other) {
                if (data_.do_free && data_.what) {
                    std::free(const_cast<char *>(data_.what));
                }

                if (other.data_.what) {
                    size_t len = std::strlen(other.data_.what) + 1;     /// 有最后的"\0"
                    char *copy = static_cast<char *>(std::malloc(len));
                    if (copy) {
                        std::strcpy(copy, other.data_.what);
                        data_.what = copy;
                        data_.do_free = true;
                    } else {
                        /// 分配char* copy失败
                        data_ = {nullptr, false};
                    }
                } else {
                    data_ = {nullptr, false};
                }
            }
            return *this;
        }

        /// 析构函数
        virtual ~exception() noexcept {
            if (data_.do_free && data_.what) {
                std::free(const_cast<char *>(data_.what));
            }
        }

        virtual const char *what() const noexcept {
            return data_.what ? data_.what : "unkonwn exception";
        }

    private:
        ExceptionData data_;
    };


    class bad_exception : public exception {
    public:
        bad_exception() noexcept: exception("std::bad_exception") {}
    };


}