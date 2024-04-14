import os
import sys
import json
import mqtt
from pathlib import Path
from aiohttp import web
from aiohttp.web import middleware
from collections import deque
from datetime import datetime, timedelta

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

# 请求频率限制参数
LIMIT_PER_INTERVAL = 1  # 每个时间间隔允许的最大请求次数
INTERVAL_LENGTH = 2  # 时间间隔，单位：秒

# 存储请求计数的队列，用于滑动窗口算法
request_counts = deque(maxlen=INTERVAL_LENGTH)

# 设置中间件
@middleware
async def app_middleware(request, handler):
    current_time = datetime.now()
    
    # 更新请求计数队列，移除过期计数
    while request_counts and request_counts[0] < current_time - timedelta(seconds=INTERVAL_LENGTH):
        request_counts.popleft()

    # 计算当前时间间隔内的请求总数
    current_count = sum(1 for _ in filter(lambda t: t >= current_time - timedelta(seconds=INTERVAL_LENGTH), request_counts))

    if current_count >= LIMIT_PER_INTERVAL:
        return web.Response(status=429, text="Too Many Requests")  # 返回429状态码，表示请求被限流

    # 添加新请求计数
    request_counts.append(current_time)

    response = await handler(request)
    response.headers['Access-Control-Allow-Origin'] = '*'
    return response

# 初始化服务器
app = web.Application(middlewares=[app_middleware])
app.router.add_get('/', handle_index)
app.router.add_get('/api/lighting/devices', get_device_list)
app.router.add_get('/api/lighting/setState', set_device_state)

if __name__ == '__main__':
    # 初始化MQTT
    with open('config.json', 'r') as config_file:
        config = json.load(config_file)
        cli.init(config['hostname'], config['username'], config['password'])

    web.run_app(app, host = '127.0.0.1', port = 5000)