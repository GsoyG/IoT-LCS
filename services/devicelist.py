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
    async def __send(self, command, subscribe = '', payload = None):
        if self.__isConnected:
            return "上一个请求的连接未关闭"

        async with aiomqtt.Client(
            self.__hostData['hostname'],
            username = self.__hostData['username'],
            password =  self.__hostData['password'],
            identifier = self.__hostData['identifier']
        ) as client:
            self.__isConnected = True

            # 订阅消息
            if subscribe != '': await client.subscribe(subscribe)
            await client.subscribe('stat/' + self.__gateway_name + '/RESULT')

            # 发布消息
            if payload is not None:
                payload = json.dumps(payload).encode('utf-8')
            topic = 'cmnd/' + self.__gateway_name + '/' + command
            await client.publish(topic, payload)
            
            # 检索响应消息
            try:
                async with asyncio.timeout(3): # 超时时间为 3 秒
                    responses = []
                    async for message in client.messages:
                        parsed_msg = json.loads(message.payload)
                        if command in parsed_msg:
                            if parsed_msg[command] != 'Done':
                                responses.append(parsed_msg[command])
                            else:
                                self.__isConnected = False
                                return responses if subscribe != '' else 'Done'
            except asyncio.TimeoutError:
                self.__isConnected = False
            return "等待请求响应超时"

    # 获取设备列表
    async def get_device_list(self):
        # 发送消息并检查返回
        subscribe = 'tele/' + self.__gateway_name + '/SENSOR'
        data = await self.__send('ZbInfo', subscribe)
        if isinstance(data, str): return data

        # 更新设备列表
        device_list = []
        for device in data:
            device_list.append(list(device.values())[0])
        return device_list
    
    # 设置设备状态信息
    async def set_device_state(self, device, state):
        # 发送消息并检查返回
        payload = { 'Device': device, 'Send': state }
        data = await self.__send('ZbSend', '', payload)
        if data != 'Done': return data

        return { 'status': 'OK'}
    
    # 设置多个设备状态信息
    async def set_device_list_state(self, device_list, state):
        for device in device_list:
            await self.set_device_state(device, state)
