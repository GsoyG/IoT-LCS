import random
import string
import aiomqtt
import json

class MqttClient:
    __gateway_name = 'ZigbeeGateway'
    __hostData = {
        'hostname': '',
        'username': '',
        'password': '',
        'identifier': '',
    }
    __device_list = []

    def init(self, hostname, username, password):
        characters = string.ascii_letters + string.digits
        id_string = ''.join(random.choice(characters) for _ in range(6))
        identifier = 'LCS_' + id_string

        self.__hostData['hostname'] = hostname
        self.__hostData['username'] = username
        self.__hostData['password'] = password
        self.__hostData['identifier'] = identifier

    async def get_device_list(self):
        async with aiomqtt.Client(self.__hostData['hostname'], username = self.__hostData['username'],
                                  password =  self.__hostData['password'], identifier = self.__hostData['identifier']) as client:
            await client.subscribe('tele/' + self.__gateway_name + '/SENSOR')

            await client.publish('cmnd/ZigbeeGateway/ZbInfo')
            async for message in client.messages:
                parsed_msg = json.loads(message.payload)
                if 'ZbInfo' in parsed_msg:
                    parsed_msg = parsed_msg['ZbInfo']
                    print(parsed_msg)
                    
                    self.__device_list.clear()
                    for key, value in parsed_msg.items():
                        self.__device_list.append(value)
                    return self.__device_list
    
    async def set_device_state(self, device, state):
        async with aiomqtt.Client(self.__hostData['hostname'], username = self.__hostData['username'],
                                  password =  self.__hostData['password'], identifier = self.__hostData['identifier']) as client:
            await client.subscribe('tele/' + self.__gateway_name + '/SENSOR')

            await client.publish('cmnd/ZigbeeGateway/ZbSend', json.dumps({
                'Device': device,
                'Send': state
            }).encode('utf-8'))
            async for message in client.messages:
                parsed_msg = json.loads(message.payload)
                if 'ZbReceived' in parsed_msg:
                    parsed_msg = parsed_msg['ZbReceived']
                    print(parsed_msg)
                    return parsed_msg[device]
