#!/usr/bin/env python3

import re
import subprocess
import argparse
import os
from typing import Dict, List, Optional, Tuple

class StackFrame:
    def __init__(self, index: int, abs_addr: str, rel_addr: str, module: str, source: str):
        self.index = index
        self.abs_addr = abs_addr
        self.rel_addr = rel_addr
        self.module = module
        self.source = source

    def needs_resolution(self) -> bool:
        return "??" in self.source

    def __str__(self) -> str:
        return f"  [{self.index}] Absolute: {self.abs_addr}, Relative: {self.rel_addr}\n" \
               f"      Module: {self.module}\n" \
               f"      Source: {self.source}"

class Lock:
    def __init__(self, address: str, thread_id: str):
        self.address = address
        self.thread_id = thread_id
        self.frames: List[StackFrame] = []
        self.waiting_for: List[str] = []  # 等待的锁地址列表

    def __str__(self) -> str:
        result = [f"\nLock {self.address} (Mutex) held by thread {self.thread_id}"]
        result.append("Acquired at:")
        for frame in self.frames:
            result.append(str(frame))
        if self.waiting_for:
            result.append("Waiting for locks: " + ", ".join(self.waiting_for))
        return "\n".join(result)

class MemoryLeak:
    def __init__(self, address: str, size: int):
        self.address = address
        self.size = size
        self.frames: List[StackFrame] = []

    def __str__(self) -> str:
        result = [f"\nLeak at {self.address} (size: {self.size} bytes)"]
        result.append("Callstack:")
        for frame in self.frames:
            result.append(str(frame))
        return "\n".join(result)

def parse_status_file(filename: str) -> Tuple[List[MemoryLeak], List[Lock]]:
    leaks: List[MemoryLeak] = []
    locks: List[Lock] = []
    current_leak: Optional[MemoryLeak] = None
    current_lock: Optional[Lock] = None
    
    with open(filename, 'r') as f:
        for line in f:
            # 匹配内存泄漏的起始行
            leak_match = re.match(r'Leak at (0x[0-9a-fA-F]+) \(size: (\d+) bytes\)', line)
            if leak_match:
                current_leak = MemoryLeak(leak_match.group(1), int(leak_match.group(2)))
                leaks.append(current_leak)
                current_lock = None
                continue

            # 匹配锁信息的起始行
            lock_match = re.match(r'Lock (0x[0-9a-fA-F]+) \(Mutex\) held by thread (\d+)', line)
            if lock_match:
                current_lock = Lock(lock_match.group(1), lock_match.group(2))
                locks.append(current_lock)
                current_leak = None
                continue

            # 匹配等待的锁
            waiting_match = re.match(r'Waiting for locks: (0x[0-9a-fA-F]+) \(held by thread \d+\)', line)
            if waiting_match and current_lock:
                current_lock.waiting_for.append(waiting_match.group(1))
                continue

            # 匹配调用栈帧
            frame_match = re.match(r'\s+\[(\d+)\] (.*) \[(0x[0-9a-fA-F]+)\]', line)
            if frame_match:
                index = int(frame_match.group(1))
                module_path = frame_match.group(2)
                abs_addr = frame_match.group(3)
                
                # 从模块路径中提取模块名
                module = os.path.basename(module_path)
                
                # 计算相对地址（这里可能需要调整）
                rel_addr = abs_addr
                
                frame = StackFrame(index, abs_addr, rel_addr, module_path, "??")
                
                if current_leak:
                    current_leak.frames.append(frame)
                elif current_lock:
                    current_lock.frames.append(frame)

    return leaks, locks

def resolve_symbols(leaks: List[MemoryLeak], locks: List[Lock], debug_dir: str) -> None:
    """使用调试符号解析地址"""
    def resolve_frames(frames: List[StackFrame]):
        for frame in frames:
            if frame.needs_resolution():
                debug_file = os.path.join(debug_dir, os.path.basename(frame.module))
                if not os.path.exists(debug_file):
                    continue
                
                try:
                    cmd = ['addr2line', '-e', debug_file, '-f', '-C', '-p', frame.rel_addr]
                    result = subprocess.run(cmd, capture_output=True, text=True)
                    if result.returncode == 0 and result.stdout.strip() and '??' not in result.stdout:
                        frame.source = result.stdout.strip()
                except subprocess.SubprocessError:
                    continue

    for leak in leaks:
        resolve_frames(leak.frames)
    
    for lock in locks:
        resolve_frames(lock.frames)

def main():
    parser = argparse.ArgumentParser(description='Analyze detector status file')
    parser.add_argument('status_file', help='Path to detector status file')
    parser.add_argument('debug_dir', help='Directory containing debug symbols')
    args = parser.parse_args()

    # 解析status文件
    print(f"Parsing {args.status_file}...")
    leaks, locks = parse_status_file(args.status_file)
    
    # 解析符号
    print(f"Resolving symbols using debug info from {args.debug_dir}...")
    resolve_symbols(leaks, locks, args.debug_dir)
    
    # 输出结果
    if leaks:
        print("\n=== Memory Tracker Status (with resolved symbols) ===")
        for leak in leaks:
            print(leak)
        print("\n===========================")

    if locks:
        print("\n=== Lock Detector Status (with resolved symbols) ===")
        for lock in locks:
            print(lock)
        print("\n===========================")

if __name__ == '__main__':
    main()