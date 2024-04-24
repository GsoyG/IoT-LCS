from aiohttp import web
from aiohttp.web import middleware
from collections import deque
from datetime import datetime, timedelta
from aiohttp_session import get_session

limit_per_interval = 60  # 每个时间间隔允许的最大请求次数
interval_length = 60  # 时间间隔，单位：秒
request_counts = deque(maxlen = interval_length) # 存储请求计数的队列，用于滑动窗口算法

# 限流中间件
@middleware
async def limit_middleware(request, handler):
    current_time = datetime.now()
    last_time = current_time - timedelta(seconds = interval_length)
    
    # 更新请求计数队列，移除过期计数
    while request_counts and request_counts[0] < last_time:
        request_counts.popleft()

    # 计算当前时间间隔内的请求总数
    current_count = sum(1 for _ in filter(lambda t: t >= last_time, request_counts))

    if current_count >= limit_per_interval:
        return web.Response(status = 429, text = "单位次数内请求次数过多")

    # 添加新请求计数
    request_counts.append(current_time)

    # 允许跨站访问
    response = await handler(request)
    response.headers['Access-Control-Allow-Origin'] = '*'
    return response

# 认证中间件
@middleware
async def auth_middleware(request, handler):
    # 登录接口以外验证身份
    if request.path != '/api/login':
        session = await get_session(request)
        if "user" not in session:
            return web.Response(status = 401, text = '没有权限访问')
    return await handler(request)
