import base64
import logging
import middlewares as mid
import aiohttp_cors
from pathlib import Path
from aiohttp import web
from aiohttp_session import setup
from cryptography.fernet import Fernet
from services.devicelist import DeviceList
from services.timingtask import TimingTask
from services.scenemode import SceneMode
from controller import log as log_controller
from controller import auth as auth_controller
from controller import device as device_controller
from controller import timing as timing_controller
from controller import scene as scene_controller
from aiohttp_session.cookie_storage import EncryptedCookieStorage

# 配置日志
logging.basicConfig(level = logging.DEBUG)
aiohttp_logger = logging.getLogger("aiohttp.server")
aiohttp_logger.setLevel(logging.DEBUG)

# 首页
async def handle_index(request):
    index_html_path = Path(__file__).parent.resolve() / 'static' / 'index.html'
    return web.FileResponse(index_html_path, headers = {
        'Cache-Control': 'no-cache',
    })

# 异步入口
async def init_app():
    # 初始化服务、控制器
    device = DeviceList()
    task = TimingTask(device)
    scene = SceneMode(device)
    device_controller.device = device
    timing_controller.task = task
    scene_controller.scene = scene
    app = web.Application(middlewares = [mid.limit_middleware])

    # 配置认证会话
    session_key = base64.urlsafe_b64decode(Fernet.generate_key())
    setup(app,  EncryptedCookieStorage(session_key, cookie_name = 'session'))
    app.middlewares.append(mid.auth_middleware)

    # 添加路由
    app.router.add_get('/', handle_index)
    app.add_routes(log_controller.routes)
    app.add_routes(auth_controller.routes)
    app.add_routes(device_controller.routes)
    app.add_routes(timing_controller.routes)
    app.add_routes(scene_controller.routes)

    # 配置 CORS 跨域访问
    cors = aiohttp_cors.setup(app, defaults = {
        "*": aiohttp_cors.ResourceOptions(
            allow_credentials = True,
            expose_headers = "*",
            allow_headers = "*",
        )
    })
    for route in list(app.router.routes()):
        cors.add(route)

    return app

# 主函数
if __name__ == '__main__':
    web.run_app(init_app(), host = '127.0.0.1', port = 5000)
