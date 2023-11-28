# 导入串口模块
import serial
# 用于计算平均值
import numpy as np
# 用于绘制图形
import matplotlib.pyplot as plt
import os



def reback(xlmin, xlmax):
    data = []
    with open("data.txt", "r") as f:  # 逐行读取文件内容
        for line in f: # 去掉每行的换行符
            line = line.strip() # 把每行的数据转换为浮点数
            value = float(line) # 把数据添加到列表中
            data.append(value)
        points = [i * 0.2 for i in range(len(data))]
        plt.figure()
        xlmin = int(xlmin)*5  # 把 min 转换为整数
        xlmax = int(xlmax)*5 # 把 max 转换为整数
        plt.plot(points[xlmin:xlmax], data[xlmin:xlmax], "b-")
        plt.title("Temperature vs Time")
        plt.xlabel("Time/s")
        plt.ylabel("Temperature (℃)")
        plt.show()

print("请选择模式\n1.绘图模式（输入1）\n2.回顾模式（输入2）")
press_mode = int(input())
if press_mode == 1:
    print("请选择绘图模式\n1.动态滚动模式（输入1）\n2.翻页模式（输入2）")
    press_in = int(input())
    print("是否开启写入文件\n1.是（输入1）\n2.否（输入2）")
    press_txt = int(input())
    print("横轴长度：")
    xllmax = int(input())
    print("纵轴长度")
    yllmin = int(input())
    yllmax = int(input())
elif press_mode == 2:
    print("选择模式\n1.回顾\n2.删除文件内容")
    press_mode = int(input())
    if press_mode == 1:
        reback(input(), input())
    elif press_mode ==2:
        with open("data.txt", "r+") as file:
            file.truncate(0)
        print("文件已清空")
        os._exit(0)  # 退出码为0
with open("data.txt", "r+") as file:
    file.truncate(0)
# 定义串口参数
port = "COM3" # 串口号，根据实际情况修改
baudrate = 9600 # 波特率，根据实际情况修改
timeout = 1 # 超时时间，单位秒

# 创建串口对象
ser = serial.Serial(port, baudrate, timeout=timeout)

# 定义数据缓存列表
data_list = []

# 定义平均值列表
avg_list = []

# 定义采样点列表
sample_list = []

def temp_convert(adc):
    return ((adc-746)/(0.000355*678)+286)/10



# 创建图形窗口
plt.figure()
# 设置横坐标标签
plt.xlabel("Time/s")
# 设置纵坐标标签
plt.ylabel("Temperature (℃)")
# 设置图形标题
plt.title("Temperature vs Time")

# 定义主循环
while True:
    # 从串口读取一行数据，以换行符为结束标志
    data = ser.readline()
    # 如果读取到数据
    if data:
        # 将数据解码为字符串，并去掉首尾空白字符
        data = data.decode().strip()
        # 如果字符串不为空
        if data:
            # 将数据转换为数值类型，float
            data = float(data)
            # 将数据添加到数据缓存列表
            data_list.append(data)
            # 其他代码省略
        else:
            # 跳过空字符串
            continue
        # 将数据转换为数值类型，float
        data = float(data)
        # 将数据添加到数据缓存列表
        data_list.append(data)
        # 如果数据缓存列表的长度达到32
        if len(data_list) == 32:
            # 计算数据缓存列表的平均值
            avg = np.mean(data_list)
            avg = temp_convert(avg)
            # 将平均值添加到平均值列表
            avg_list.append(avg)
            if press_txt == 1:
                f = open("data.txt", "a")
                f.write(str(avg) + "\n")
                f.close()
            sample = len(avg_list)
            # 将采样点添加到采样点列表
            sample_list.append(sample * 0.2) # 修改这里，使得每个采样点的序号乘以 0.2
            # 清空数据缓存列表
            data_list = []
            # 打印平均值和采样点
            print(f"Average: {avg}, Sample: {sample}")
            # 开启交互模式
            plt.ion()
            # 绘制图形
            plt.plot(sample_list, avg_list, "b-") # 用蓝色实线绘制平均值曲线
            if press_in == 1:
                # 获取采样点列表的最大值和最小值
                xmax = max(sample_list)
                xmin = xmax - xllmax
                if xmin > 0:
                    plt.xlim(xmin, xmax)
                else:
                    plt.xlim(0.2, xllmax)  # 修改这里，使得横坐标的最小值是 0.2
            elif press_in == 2:
                plt.xlim(xllmax * int((max(sample_list) / xllmax)) + 0.2,
                         xllmax * int((max(sample_list) / xllmax + 1)))  # 修改这里，使得横坐标的最小值是 0.2 的倍数
            else:
                # 处理其他情况
                print("无效的模式，请重新输入")
                break
            # 设置纵坐标的范围
            plt.ylim(yllmin, yllmax)
            # 更新图形
            plt.draw()
            # 暂停 0.01 秒
            plt.pause(0.01)
    # 如果没有读取到数据
    else:
        # 跳出循环
        break
# 显示图形
plt.show(block=False)
