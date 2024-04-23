import json
from pathlib import Path
from aiohttp import web
from mqttclient import MqttClient
from httpserver import HttpServer
from timingtask import TimingTask

cli = MqttClient()
task = TimingTask()

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
    try: # 检查参数
        device = request.query['device']
        state = json.loads(request.query['state'])
    except KeyError:
        return web.Response(status = 400, text = '参数错误：找不到指定参数')
    
    # 设置设备状态
    data = await cli.set_device_state_fast(device, state)
    if isinstance(data, str):
        return web.Response(status = 429, text = data)
    return web.json_response(data)

# 获取定时任务列表
async def get_timing_task(request):
    return web.json_response(task.get_task_list())

# 设置定时任务
async def set_timing_task(request):
    try: # 检查参数
        action = request.query['action']
        data = json.loads(request.query['data'])
    except KeyError:
        return web.Response(status = 400, text = '参数错误：找不到指定参数')
    
    # 操作定时任务项
    if action == 'add':
        result = task.add_task(data)
    elif action == 'delete':
        result = task.delete_task(data)
    elif action == 'update':
        result = task.update_task(data)
    else: return web.Response(status = 400, text = '参数错误：未知操作')
    
    if result != 'OK':
        return web.Response(status = 400, text = result)
    return web.json_response({ 'status': 'OK'})

if __name__ == '__main__':
    with open('config.json', 'r') as config_file:
        config = json.load(config_file)
        cli.init(config['hostname'], config['username'], config['password'])

    srv = HttpServer()
    srv.app.router.add_get('/', handle_index)
    srv.app.router.add_get('/api/lighting/devices', get_device_list)
    srv.app.router.add_get('/api/lighting/setState', set_device_state)

    srv.app.router.add_get('/api/timing/tasks', get_timing_task)
    srv.app.router.add_get('/api/timing/setTask', set_timing_task)
    srv.run()
