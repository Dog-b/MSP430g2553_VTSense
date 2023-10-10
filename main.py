import serial  # 导入串口模块
import matplotlib.pyplot as plt  # 导入绘图模块
import scipy.ndimage  # 导入科学计算模块

ser = serial.Serial('COM3', 9600, timeout=1)  # 打开串口，设置波特率为 9600，超时时间为 1 秒
ser.flushInput()  # 清空串口输入缓冲区

x = []  # 存储时间数据的列表
y = []  # 存储电压数据的列表

plt.ion()  # 开启交互模式

while True:  # 循环读取串口数据并绘图
    try:
        data = ser.readline().decode().strip()  # 读取一行串口数据，解码为字符串，并去掉首尾空白字符
        if data:  # 如果数据不为空
            voltage = int(data) * 2.5 / 1023  # 将数据转换为整数，并计算电压值，假设参考电压为 2.5，分辨率为 10 位
            print(voltage)  # 打印电压值

            x.append(len(x))  # 将时间数据添加到列表 x 中，假设每次采样间隔为 1 单位时间
            y.append(voltage)  # 将电压数据添加到列表 y 中

            plt.clf()  # 清除当前图形

            sigma = 5  # 设置高斯平滑的 sigma 参数，值越大，平滑程度越高
            y_smooth = scipy.ndimage.gaussian_filter1d(y, sigma)  # 对 y 数据进行高斯平滑，得到平滑后的数据 y_smooth

            plt.plot(x, y, 'b-', alpha=0.5)  # 绘制原始的时间-电压曲线，颜色为蓝色，线型为实线，透明度为 0.5
            plt.plot(x, y_smooth, 'r-')  # 绘制平滑后的时间-电压曲线，颜色为红色，线型为实线
            plt.xlabel('Time')  # 设置 x 轴标签为 Time
            plt.ylabel('Voltage')  # 设置 y 轴标签为 Voltage
            plt.title('Voltage Sensor Data')  # 设置图形标题为 Voltage Sensor Data

            plt.pause(0.01)  # 暂停 0.01 秒，用于更新图形

    except KeyboardInterrupt:  # 如果按下 Ctrl+C 键，则
        break  # 跳出循环

ser.close()  # 关闭串口
plt.ioff()  # 关闭交互模式
plt.show()  # 显示最终的图形
