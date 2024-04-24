import os
import sys
import json
import random
import string
import asyncio
import aiomqtt

# 在Windows平台下改变事件循环策略，避免报错
if sys.platform.lower() == "win32" or os.name.lower() == "nt":
    from asyncio import set_event_loop_policy, WindowsSelectorEventLoopPolicy
    set_event_loop_policy(WindowsSelectorEventLoopPolicy())

class DeviceList:
    __gateway_name = 'ZigbeeGateway'
    __hostData = {
        'hostname': '',
        'username': '',
        'password': '',
        'identifier': '',
    }
    __device_list = []
    __isConnected = False

    # 初始化客户端字段
    def __init__(self):
        # 生成ID
        characters = string.ascii_letters + string.digits
        id_string = ''.join(random.choice(characters) for _ in range(6))
        identifier = 'LCS_' + id_string
        self.__hostData['identifier'] = identifier
        
        # 读取配置
        with open('config.json', 'r') as config_file:
            config = json.load(config_file)
            self.__hostData['hostname'] = config['hostname']
            self.__hostData['username'] = config['username']
            self.__hostData['password'] = config['password']

    # 发送消息并返回响应
    async def __send(self, command, subscribe, message_header, payload = None):
        if self.__isConnected:
            return "上一个请求的连接未关闭"

        async with aiomqtt.Client(
            self.__hostData['hostname'],
            username = self.__hostData['username'],
            password =  self.__hostData['password'],
            identifier = self.__hostData['identifier']
        )as client:
            self.__isConnected = True

            # 订阅消息
            await client.subscribe(subscribe)

            # 发布消息
            if payload is not None:
                payload = json.dumps(payload).encode('utf-8')
            topic = 'cmnd/' + self.__gateway_name + '/' + command
            await client.publish(topic, payload)
            
            # 检索响应消息
            try:
                async with asyncio.timeout(3): # 超时时间为 3 秒
                    async for message in client.messages:
                        parsed_msg = json.loads(message.payload)
                        if message_header in parsed_msg:
                            self.__isConnected = False
                            return parsed_msg[message_header]
            except asyncio.TimeoutError:
                self.__isConnected = False
            return "等待请求响应超时"

    # 获取设备列表
    async def get_device_list(self):
        subscribe = 'tele/' + self.__gateway_name + '/SENSOR'
        data = await self.__send('ZbInfo', subscribe, 'ZbInfo')
        if isinstance(data, str):
            return data

        # 更新本地设备列表
        self.__device_list.clear()
        for key, value in data.items():
            self.__device_list.append(value)
        return self.__device_list
    
    # 设置设备状态信息
    async def set_device_state(self, device, state):
        subscribe = 'tele/' + self.__gateway_name + '/SENSOR'
        payload = {
            'Device': device,
            'Send': state
        }
        data = await self.__send('ZbSend', subscribe, 'ZbReceived', payload)
        if isinstance(data, str):
            return data
        return data[device]
    
    # 设置设备状态信息，无需等待响应消息，响应快两倍
    async def set_device_state_fast(self, device, state):
        subscribe = 'stat/' + self.__gateway_name + '/RESULT'
        payload = {
            'Device': device,
            'Send': state
        }
        data = await self.__send('ZbSend', subscribe, 'ZbSend', payload)
        if data != 'Done':
            return data
        return { 'status': 'OK'}
    
    # 设置多个设备状态信息
    async def set_device_list_state(self, device_list, state):
        for device in device_list:
            await self.set_device_state_fast(device, state)
