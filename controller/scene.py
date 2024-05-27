import json
import services.logger as logger
from aiohttp import web
from aiohttp_session import get_session
from services.scenemode import SceneMode

scene: SceneMode = None
routes = web.RouteTableDef()

# 获取场景模式列表
@routes.get('/api/scene/modes')
async def get_scene_modes(request):
    return web.json_response(scene.get_mode_list())

# 设置场景模式
@routes.get('/api/scene/setMode')
async def set_scene_mode(request):
    try: # 检查参数
        action = request.query['action']
        data = json.loads(request.query['data'])
    except KeyError:
        return web.Response(status = 400, text = '参数错误：找不到指定参数')
    
    # 操作场景模式项
    if action == 'add':
        result = scene.add_mode(data)
    elif action == 'delete':
        result = scene.delete_mode(data)
    elif action == 'update':
        result = scene.update_mode(data)
    else: return web.Response(status = 400, text = '参数错误：未知操作')
    
    session = await get_session(request)
    if result != 'OK':
        logger.write_log(session["user"]['username'], request.remote, '场景模式', f'设置模式失败，{result}')
        return web.Response(status = 400, text = result)
    logger.write_log(session["user"]['username'], request.remote, '场景模式', f'设置模式成功，模式：{data['name']}')
    return web.json_response({ 'status': 'OK' })

# 切换场景模式
@routes.get('/api/scene/switchMode')
async def switch_scene_mode(request):
    try: # 检查参数
        name = request.query['name']
    except KeyError:
        return web.Response(status = 400, text = '参数错误：找不到指定参数')
    
    result = scene.switch_mode(name)
    session = await get_session(request)
    if result != 'OK':
        logger.write_log(session["user"]['username'], request.remote, '场景模式', f'切换模式失败，{result}')
        return web.Response(status = 400, text = result)
    logger.write_log(session["user"]['username'], request.remote, '场景模式', f'切换模式成功，模式：{name}')
    return web.json_response({ 'status': 'OK' })
