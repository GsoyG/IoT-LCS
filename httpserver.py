import logging
from aiohttp import web
from aiohttp.web import middleware
from collections import deque
from datetime import datetime, timedelta

# 配置 logging
logging.basicConfig(level=logging.DEBUG)
aiohttp_logger = logging.getLogger("aiohttp.server")
aiohttp_logger.setLevel(logging.DEBUG)

class HttpServer:
    app = None

    __limit_per_interval = 60  # 每个时间间隔允许的最大请求次数
    __interval_length = 60  # 时间间隔，单位：秒
    __request_counts = deque(maxlen = __interval_length) # 存储请求计数的队列，用于滑动窗口算法

    # 中间件
    @middleware
    async def middleware(self, request, handler):
        current_time = datetime.now()
        last_time = current_time - timedelta(seconds = self.__interval_length)
        
        # 更新请求计数队列，移除过期计数
        while self.__request_counts and self.__request_counts[0] < last_time:
            self.__request_counts.popleft()

        # 计算当前时间间隔内的请求总数
        current_count = sum(1 for _ in filter(lambda t: t >= last_time, self.__request_counts))

        if current_count >= self.__limit_per_interval:
            return web.Response(status = 429, text = "单位次数内请求次数过多")

        # 添加新请求计数
        self.__request_counts.append(current_time)

        response = await handler(request)
        response.headers['Access-Control-Allow-Origin'] = '*'
        return response
