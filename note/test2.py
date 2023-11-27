# 导入所需的模块
import serial # 用于串口通信
import numpy as np # 用于计算平均值
import matplotlib.pyplot as plt # 用于绘制图形

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

# 定义温度转换函数，根据msp430g2553的ADC规格书，温度传感器的输出电压与温度的关系为：
# Vout = 0.00355 * Temp + 0.986
# 其中，Vout为输出电压，单位伏特；Temp为温度，单位摄氏度
# 因此，温度可以由输出电压反推得到：
# Temp = (Vout - 0.986) / 0.00355
# 其中，Vout可以由ADC的输出值计算得到：
# Vout = ADC * 3.3 / 1024
# 其中，ADC为ADC的输出值，范围为0-1023；3.3为参考电压，单位伏特；1024为ADC的分辨率
# 综上，温度可以由ADC的输出值计算得到：
# Temp = (ADC * 3.3 / 1024 - 0.986) / 0.00355
def temp_convert(adc):
    return ((adc-746)/(0.000355*678)+286)/10

# 定义主循环
while True:
    # 从串口读取一行数据，以换行符为结束标志
    data = ser.readline()
    # 如果读取到数据
    if data:
        # 将数据解码为字符串，并去掉首尾空白字符
        data = data.decode().strip()
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
            # 计算采样点，即平均值列表的长度
            sample = len(avg_list)
            # 将采样点添加到采样点列表
            sample_list.append(sample)
            # 清空数据缓存列表
            data_list = []
            # 打印平均值和采样点
            print(f"Average: {avg}, Sample: {sample}")
            # 开启交互模式
            plt.ion()
            # 绘制图形
            plt.plot(sample_list, avg_list, "b-") # 用蓝色实线绘制平均值曲线
            plt.xlabel("Sample") # 设置横坐标标签为Sample
            plt.ylabel("ADC") # 设置纵坐标标签为ADC
            plt.title("ADC vs Sample") # 设置图形标题为ADC vs Sample
            plt.show() # 显示图形
            # 暂停 0.01 秒
            plt.pause(0.01)
    # 如果没有读取到数据
    else:
        # 跳出循环
        break

# 关闭串口对象
ser.close()
