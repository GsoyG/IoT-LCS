# 智能照明控制系统
本项目主要采用 Vue 作为前端，Python 作为后端。通过控制刷入 Tasmota 的 ESP 系列 MCU 网关，实现控制 Zigbee 照明设备。<br>
系统核心部分主要通过使用 MQTT 协议，与 Tasmota 网关通信，通过 Tasmota 网关控制 Zigbee 照明设备。<br><br>
系统架构概览：<br>
用户 =>【HTTP 服务器 => MQTT 客户端】=> MQTT 服务端 =>【MQTT 客户端 => Tasmota 网关】=> Zigbee 照明设备<br>
其中
- 【HTTP 服务器 => MQTT 客户端】：为本项目部分
-  MQTT 服务端：为第三方 MQTT 服务器（MQTT Broker），例如 [Mosquitto](https://mosquitto.org/) 、[EMQX](https://www.emqx.io/)
- 【MQTT 客户端 => Tasmota 网关】：为 [Tasmota](https://tasmota.github.io/)  网关部分
- Zigbee 照明设备：为[终端设备](./zstack/README.md)部分

## 🖥️ 终端介绍
参考 `/zstack` 目录下的 [README.md](./zstack/README.md)

## 🖥️ 前端介绍
参考 `/LCS-Frontend` 目录下的 [README.md](./LCS-Frontend/README.md)

## 🖥️ 后端介绍
使用 Python3 开发
- Aiohttp 实现 HTTP 服务器
- Aiomttq 实现 MQTT 客户端
- TinyDB 实现数据库存储
- Apscheduler 实现定时任务

### 文件结构
- `/zstack` : Z-Stack 终端项目，主要采用了 Z-Stack Zigbee 协议框架
- `/LCS-Frontend` : Vue3 前端项目，主要采用了 Vuetify UI 框架。
- `/controller` : 控制层，负责调用 services 实现后端接口逻辑
- `/services` : 服务层，实现主要控制代码

### 运行项目
#### 1. 安装依赖
```shell
pip install -r requirements.txt
```
#### 2. 运行
- 准备 MQTT 服务器配置，创建文件 `./config.json`
```json
{
    "hostname": "your hostname",
    "username": "your username",
    "password": "your password"
}
```
- 执行 `main.py` ，程序将在运行目录创建数据库文件 `./database.json`
```shell
python main.py
```
