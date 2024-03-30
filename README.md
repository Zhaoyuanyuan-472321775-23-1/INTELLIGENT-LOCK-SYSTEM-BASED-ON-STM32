# 基于STM32的智能锁系统
## 智能锁系统开机示意图
<div ><img src="https://github.com/zyysin/INTELLIGENT-LOCK-SYSTEM-BASED-ON-STM32/blob/main/img/472321775.png" width="400" height="400" /></div>

## 系统功能简介
随着当今科技的迅猛发展，传统的身份识别方式因其拥有与用户身份的可分离性，已经不能够满足现代社会的需求。

用户需要更加安全、可靠与高效的方式进行身份识别以保障自身的个人利益得到保护，基于人脸与指纹识别的智能锁系统便因此而诞生。

# 该设计采用STM32作为系统的主处理器，采用人脸识别、指纹识别以及密码判别三种方式进行用户的身份识别。

# 在信息采集阶段，系统通过在K210处理器编写的YOLO算法进行人脸信息的目标检测。

通过指纹识别模块采集指纹信息，并采用矩阵键盘进行密码信息的采集工作。

上述身份信息经过采集之后将实时传输到STM32处理器进行数据的处理与信息的比对，身份信息比对成功后系统方可开锁。

## 系统软件安装版本
KeiluVision5

Maix Py IDE

PZ-ISP

Kflash

## 系统硬件选型
STM32F103C8T6

K210

ATK-AS608

4×4的薄膜矩阵键盘

Risym的1路5V继电器
##  系统各部件连接完成的实机图
<div ><img src="https://github.com/zyysin/INTELLIGENT-LOCK-SYSTEM-BASED-ON-STM32/blob/main/img/sys.PNG" width="500" height="400" /></div>

## 目录结构描述
    ├── ReadMe.md          // 帮助文档
    
    ├── CORE              // 内核支持API，提供了汇编级函数的实现
    
    ├── HARDWAR           // 包括编译硬件的代码文件
    
    ├── OBJ               // 包括程序编译成功之后生成的hex文件
    
    ├── SYSTEM            // 包括中断，延时与同步异步收发功能的代码文件
    
    └── USER              // 包括将各硬件进行连接的代码文件               
