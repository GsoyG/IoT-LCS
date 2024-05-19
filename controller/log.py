import json
from aiohttp import web
import services.logger as logger

routes = web.RouteTableDef()

# 查询日志
@routes.get('/api/log/logs')
async def get_logs(request):
    try: # 检查参数
        offset = int(request.query['offset'])
        count = int(request.query['count'])
        filter = json.loads(request.query['filter'])
    except KeyError:
        return web.Response(status = 400, text = '参数错误：找不到指定参数')
    
    logs = logger.get_logs(
        filter.get('start_time'),
        filter.get('end_time'),
        filter.get('user'),
        filter.get('address'),
        filter.get('type'),
        filter.get('message')
    )
    logs_len = len(logs)

    if offset >= logs_len:
        return web.json_response({ 'logs': [], 'total': 0 })
    
    # 处理列表长度，最多返回20条
    if count > 20: count = 20
    if offset + count > logs_len:
        count = logs_len
    else: count = offset + count

    return web.json_response({ 'logs': logs[offset : count], 'total': logs_len })
