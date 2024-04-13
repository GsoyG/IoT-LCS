import os
import sys
import json
import mqtt
from pathlib import Path
from aiohttp import web

# 在Windows平台下改变事件循环策略，避免报错
if sys.platform.lower() == "win32" or os.name.lower() == "nt":
    from asyncio import set_event_loop_policy, WindowsSelectorEventLoopPolicy
    set_event_loop_policy(WindowsSelectorEventLoopPolicy())

cli = mqtt.MqttClient()

base_dir = Path(__file__).parent.resolve()

# 首页
async def handle_index(request):
    index_html_path = base_dir / 'static' / 'index.html'

    return web.FileResponse(index_html_path, headers={
        'Cache-Control': 'no-cache',
    })

# 获取设备列表
async def get_device_list(request):
    return web.json_response(await cli.get_device_list())

# 设置设备状态
async def set_device_state(request):
    device = request.query['device']
    state = json.loads(request.query['state'])
    return web.json_response(await cli.set_device_state(device, state)
)

# 设置中间件
async def cors_middleware(app, handler):
    async def middleware_handler(request):
        response = await handler(request)
        response.headers['Access-Control-Allow-Origin'] = '*'
        return response
    return middleware_handler

# 初始化服务器
app = web.Application(middlewares=[cors_middleware])
app.router.add_get('/', handle_index)
app.router.add_get('/api/lighting/devices', get_device_list)
app.router.add_get('/api/lighting/setState', set_device_state)

if __name__ == '__main__':
    # 初始化MQTT
    with open('config.json', 'r') as config_file:
        config = json.load(config_file)
        cli.init(config['hostname'], config['username'], config['password'])

    web.run_app(app, host = '127.0.0.1', port = 5000)