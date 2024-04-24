import json
from aiohttp import web
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
    
    if result != 'OK':
        return web.Response(status = 400, text = result)
    return web.json_response({ 'status': 'OK' })
