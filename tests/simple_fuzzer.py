import subprocess
import random
import string
import json
import os
import time
import sys

# 配置
EXECUTABLE_PATH = os.path.join("..", "build", "finance_manager.exe") # 假设这是你的主程序
TEST_DURATION = 60  # 测试持续时间（秒）
LOG_FILE = "fuzz_log.txt"

def generate_random_string(length=10):
    return ''.join(random.choices(string.ascii_letters + string.digits, k=length))

def generate_fuzz_input():
    """生成随机的 JSON 交易数据"""
    # 50% 概率生成合法结构但随机值，50% 概率生成完全畸形数据
    if random.random() < 0.5:
        data = {
            "amount": random.uniform(-10000, 10000),
            "type": random.choice(["INCOME", "EXPENSE", "INVALID"]),
            "category": generate_random_string(5),
            "date": generate_random_string(10), # 故意使用错误格式
            "note": generate_random_string(50)
        }
        return json.dumps(data)
    else:
        # 完全随机的字节流
        return generate_random_string(random.randint(10, 100))

def run_fuzzer():
    if not os.path.exists(EXECUTABLE_PATH):
        print(f"错误: 找不到可执行文件 {EXECUTABLE_PATH}")
        print("请先编译项目: cd build; cmake --build .")
        return

    print(f"开始模糊测试，持续 {TEST_DURATION} 秒...")
    print(f"目标程序: {EXECUTABLE_PATH}")
    
    start_time = time.time()
    iterations = 0
    crashes = 0

    with open(LOG_FILE, "w") as log:
        while time.time() - start_time < TEST_DURATION:
            fuzz_data = generate_fuzz_input()
            iterations += 1
            
            # 将数据写入临时文件作为输入，或者通过管道传递
            # 这里假设程序从 stdin 读取，或者我们可以修改程序接受文件参数
            # 为了演示，我们尝试通过管道传入数据
            
            try:
                # 运行程序，通过 stdin 传入模糊数据
                process = subprocess.Popen(
                    [EXECUTABLE_PATH],
                    stdin=subprocess.PIPE,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    text=True
                )
                
                stdout, stderr = process.communicate(input=fuzz_data, timeout=1)
                
                if process.returncode != 0:
                    # 记录非零退出码（可能是崩溃，也可能是正常错误处理）
                    # 在 Windows 上，崩溃通常会有特定的负退出码
                    if process.returncode < 0 or process.returncode == 3221225477: # Access Violation
                        print(f"发现潜在崩溃! 退出码: {process.returncode}")
                        log.write(f"CRASH INPUT: {fuzz_data}\n")
                        log.write(f"EXIT CODE: {process.returncode}\n")
                        crashes += 1
            
            except subprocess.TimeoutExpired:
                process.kill()
            except Exception as e:
                print(f"执行错误: {e}")

            if iterations % 100 == 0:
                print(f"已执行 {iterations} 次测试...", end="\r")

    print(f"\n测试结束。")
    print(f"总执行次数: {iterations}")
    print(f"发现崩溃数: {crashes}")
    print(f"日志已保存至 {LOG_FILE}")

if __name__ == "__main__":
    run_fuzzer()
