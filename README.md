# VFD Clock with HNM-18LM05T

## 版本
目前PCB一共两个版本，红色为旧版本，白色为新版本。旧版本由于疏忽导致部分io线没画，采用飞线连接。新版已修复。
> 下载固件时旧版本请下载**oldboard**开头的固件
  新版对应**newboard**开头固件
  
## 操作说明
### 固件更新
- 请参照**帮助文档**进行固件更新，没有给51单片机刷过固件的朋友，建议查阅51冷启动刷固件教程。请先**严格按照帮助文档内容进行操作**！如果有异常再联系我。
### 开机设置
> **每次刷完**固件都要做一次参数设置，不然由于固件更新需要擦出flash，而掉电存储参数又是使用flash模拟的eeprom，可能会将之前存储数据地方修改，导致开机运行异常。此时按照注意事项和帮助文档将所有参数全部设置一遍即可。

#### 如何设置
1.使用板载三个按键设置，三个按键逻辑功能分别为加、减、set。具体映射关系请参照板载丝印，或者逐个尝试。
2.使用遥控器设置，由于尚未添加红外学习功能，遥控器设置功能的支持需要单独定制固件，具体请看*项目工程下帮助文档*。并且关于按键和遥控器功能暂时优先维护按键操作的。
#### 注意事项
- 帮助文档内的图片不定期更新，可能有些内容会滞后，具体请自行操作对比
### 功能
- 设置支持20s无操作自动返回常规界面，此时当前做过的修改不保证全部生效。要想保证全部生效必须手动按完一圈流程。
> 按完一圈是指从按下set键，到返回正常显示，这样全部参数都修改生效，此时再多按set，可依靠20s自动返回显示界面。
- 设置年支持2000-2099。
- 设置的参数支持上下限切换，比如12月再加自动变为1月。00时减自动变为23时。每个参数的上下限都有考虑到。
- set pos 用于设置时间秒和温度显示的位置，具体效果自行尝试。
- 关于闹钟，启用闹钟时TIMER会常亮
- 关于自动亮度，当处于**自动模式时PROBE会常亮**，手动模式PROBE熄灭。PT6324支持的是8级亮度，实际调试发现这款vfd屏幕仅仅有4级的区别，就是说5、6、7、8和4是一样亮的，所以只有4级亮度。

