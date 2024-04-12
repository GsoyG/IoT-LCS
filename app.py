import os
import sys
import json
import mqtt
from pathlib import Path
from aiohttp import web

# Change to the "Selector" event loop if platform is Windows
if sys.platform.lower() == "win32" or os.name.lower() == "nt":
    from asyncio import set_event_loop_policy, WindowsSelectorEventLoopPolicy
    set_event_loop_policy(WindowsSelectorEventLoopPolicy())

cli = mqtt.MqttClient()

base_dir = Path(__file__).parent.resolve()

async def handle_index(request):
    index_html_path = base_dir / 'static' / 'index.html'

    return web.FileResponse(index_html_path, headers={
        'Cache-Control': 'no-cache',
    })

async def get_lighting_devices(request):
    headers = {'Content-Type': 'application/json'}
    json_data = json.dumps(await cli.get_device_list())
    return web.Response(body = json_data.encode('utf-8'), headers = headers, status = 200)

app = web.Application()
app.router.add_get('/', handle_index)
app.router.add_get('/api/lighting/devices', get_lighting_devices)

if __name__ == '__main__':
    with open('config.json', 'r') as config_file:
        config = json.load(config_file)
        cli.init(config['hostname'], config['username'], config['password'])

    web.run_app(app, host = '127.0.0.1', port = 5000)