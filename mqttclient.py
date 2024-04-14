import os
import sys
import json
import random
import string
import aiomqtt

# 在Windows平台下改变事件循环策略，避免报错
if sys.platform.lower() == "win32" or os.name.lower() == "nt":
    from asyncio import set_event_loop_policy, WindowsSelectorEventLoopPolicy
    set_event_loop_policy(WindowsSelectorEventLoopPolicy())

class MqttClient:
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
    def init(self, hostname, username, password):
        characters = string.ascii_letters + string.digits
        id_string = ''.join(random.choice(characters) for _ in range(6))
        identifier = 'LCS_' + id_string

        self.__hostData['hostname'] = hostname
        self.__hostData['username'] = username
        self.__hostData['password'] = password
        self.__hostData['identifier'] = identifier

    # 发送消息并返回响应
    async def send(self, topic, subscribe, message_header, payload = None):
        if self.__isConnected == True:
            return None

        async with aiomqtt.Client(self.__hostData['hostname'], username = self.__hostData['username'],
                                  password =  self.__hostData['password'],
                                  identifier = self.__hostData['identifier']) as client:
            self.__isConnected = True

            # 订阅消息
            await client.subscribe(subscribe)

            # 发布消息
            if payload is not None:
                payload = json.dumps(payload).encode('utf-8')
            await client.publish(topic, payload)
            
            # 检索响应消息
            async for message in client.messages:
                parsed_msg = json.loads(message.payload)
                if message_header in parsed_msg:
                    self.__isConnected = False
                    return parsed_msg[message_header]

    async def get_device_list(self):
        subscribe = 'tele/' + self.__gateway_name + '/SENSOR'
        data = await self.send('cmnd/ZigbeeGateway/ZbInfo', subscribe, 'ZbInfo')
        if data is None:
            return None

        # 更新设备列表
        self.__device_list.clear()
        for key, value in data.items():
            self.__device_list.append(value)
        return self.__device_list
    
    async def set_device_state(self, device, state):
        subscribe = 'tele/' + self.__gateway_name + '/SENSOR'
        payload = {
            'Device': device,
            'Send': state
        }
        data = await self.send('cmnd/ZigbeeGateway/ZbSend', subscribe, 'ZbReceived', payload)
        if data is None:
            return None
        return data[device]
