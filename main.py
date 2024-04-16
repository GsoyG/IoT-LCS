import json
from pathlib import Path
from aiohttp import web
from mqttclient import MqttClient
from httpserver import HttpServer

cli = MqttClient()

# 首页
async def handle_index(request):
    index_html_path = Path(__file__).parent.resolve() / 'static' / 'index.html'

    return web.FileResponse(index_html_path, headers = {
        'Cache-Control': 'no-cache',
    })

# 获取设备列表
async def get_device_list(request):
    data = await cli.get_device_list()
    if isinstance(data, str):
        return web.Response(status = 429, text = data)
    return web.json_response(data)

# 设置设备状态
async def set_device_state(request):
    device = request.query['device']
    state = json.loads(request.query['state'])
    data = await cli.set_device_state_fast(device, state)
    if isinstance(data, str):
        return web.Response(status = 429, text = data)
    return web.json_response(data)

if __name__ == '__main__':
    with open('config.json', 'r') as config_file:
        config = json.load(config_file)
        cli.init(config['hostname'], config['username'], config['password'])

    srv = HttpServer()
    srv.app.router.add_get('/', handle_index)
    srv.app.router.add_get('/api/lighting/devices', get_device_list)
    srv.app.router.add_get('/api/lighting/setState', set_device_state)
    srv.run()
