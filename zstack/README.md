# 智能照明控制系统 -终端-
本项目主要基于 TI 公司的 Z-Stack 协议库开发

## 编译项目
### 1. 安装 Z-Stack
在 [TI Z-Stack](https://www.ti.com/tool/Z-STACK) 下载 Z-Stack 并安装
### 2. 配置环境变量
找到 Z-Stack 安装目录<br>
将安装目录配置为 `ZSTACK` 环境变量，其中应该包含 `Components` 文件夹，例如：
```
ZSTACK = C:\Texas Instruments\Z-Stack 3.0.2
```
将安装目录下的 `\Projects\zstack` 目录配置为 ZSTACK_PROJECTS 环境变量，其中应该包含 `HomeAutomation` 文件夹，例如：
```
ZSTACK_PROJECTS = C:\Texas Instruments\Z-Stack 3.0.2\Projects\zstack
```
### 3. 编译项目
使用 IAR 打开工程并点击 `Make`
