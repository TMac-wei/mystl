/**
 * @file      my_void_t.h
 * @brief     [用于获取 void 类型]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

/// 用于获取 void 类型, C++17 中引入，主要用来做模板类型推导
template <typename...>
using void_t = void;
