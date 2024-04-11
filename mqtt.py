import os
import sys
import random
import string
import asyncio
import aiomqtt
import json

# Change to the "Selector" event loop if platform is Windows
if sys.platform.lower() == "win32" or os.name.lower() == "nt":
    from asyncio import set_event_loop_policy, WindowsSelectorEventLoopPolicy
    set_event_loop_policy(WindowsSelectorEventLoopPolicy())

class MqttClient:
    __gateway_name = "ZigbeeGateway"
    __device_list = []

    async def __init(self, hostname, username, password):
        characters = string.ascii_letters + string.digits
        id_string = ''.join(random.choice(characters) for _ in range(6))
        identifier = "LCS_" + id_string

        async with aiomqtt.Client(hostname, username = username, password = password, identifier = identifier) as client:
            await client.subscribe("tele/" + self.__gateway_name + "/SENSOR")

            await client.publish("cmnd/ZigbeeGateway/ZbInfo")
            async for message in client.messages:
                print(message.payload)

                parsed_msg = json.loads(message.payload)
                if not "ZbInfo" in parsed_msg:
                    continue
                
                parsed_msg = parsed_msg["ZbInfo"]
                for key, value in parsed_msg.items():
                    self.__device_list.append(value)

    def init(self, hostname, username, password):
        asyncio.run(self.__init(hostname, username, password))
