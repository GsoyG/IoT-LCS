import json
import services.logger as logger
from aiohttp import web
from aiohttp_session import get_session
from services.timingtask import TimingTask

task: TimingTask = None
routes = web.RouteTableDef()

# 获取定时任务列表
@routes.get('/api/timing/tasks')
async def get_timing_task(request):
    return web.json_response(task.get_task_list())

# 设置定时任务
@routes.get('/api/timing/setTask')
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
    
    session = await get_session(request)
    if result != 'OK':
        logger.write_log(session["user"]['username'], request.remote, '定时任务', f'设置任务失败，{result}')
        return web.Response(status = 400, text = result)
    logger.write_log(session["user"]['username'], request.remote, '定时任务', f'设置任务成功，任务：{data['name']}')
    return web.json_response({ 'status': 'OK' })
