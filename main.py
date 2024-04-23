import json
from pathlib import Path
from aiohttp import web
from mqttclient import MqttClient
from httpserver import HttpServer
from timingtask import TimingTask
from aiohttp_session import get_session, new_session

cli = MqttClient()
srv = HttpServer()
task = TimingTask(cli)

# 首页
async def handle_index(request):
    index_html_path = Path(__file__).parent.resolve() / 'static' / 'index.html'

    return web.FileResponse(index_html_path, headers = {
        'Cache-Control': 'no-cache',
    })

# 登录
async def handle_login(request):
    try: # 检查参数
        data = await request.json()
    except json.decoder.JSONDecodeError:
        return web.Response(status = 400, text = '参数错误：参数格式错误')
    
    username = data.get('username')
    password = data.get('password')
    if not (username and password):
        return web.Response(status = 400, text = '参数错误：缺少用户名或密码')

    if username == 'admin' and password == 'password':
        session = await new_session(request)
        session['user'] = { 'username': username }
        return web.json_response({ 'status': 'OK' })
    else: return web.Response(status = 401, text = '用户名或密码错误')

# 登出
async def handle_logout(request):
    session = await get_session(request)
    if "user" in session:
        del session["user"]
    return web.json_response({ 'status': 'OK' })

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
    return web.json_response({ 'status': 'OK' })

# 异步入口
async def init_app():
    app = web.Application(middlewares = [srv.limit_middleware])
    srv.setup_session(app)
    # app.middlewares.append(srv.auth_middleware)

    app.router.add_get('/', handle_index)
    app.router.add_post('/api/login', handle_login)
    app.router.add_post("/api/logout", handle_logout)

    app.router.add_get('/api/lighting/devices', get_device_list)
    app.router.add_get('/api/lighting/setState', set_device_state)

    app.router.add_get('/api/timing/tasks', get_timing_task)
    app.router.add_get('/api/timing/setTask', set_timing_task)

    task.setup_scheduler()

    return app

# 主函数
if __name__ == '__main__':
    with open('config.json', 'r') as config_file:
        config = json.load(config_file)
        cli.init(config['hostname'], config['username'], config['password'])
    
    web.run_app(init_app(), host = '127.0.0.1', port = 5000)
