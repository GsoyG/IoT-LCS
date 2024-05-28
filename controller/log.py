import json
import time
import services.logger as logger
from aiohttp import web
from aiohttp_session import get_session

routes = web.RouteTableDef()

# 记录写入日志时间
__get_log_record = {
    'user': {
        'ip': '',
        'time': 0,
    },
}

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

    session = await get_session(request)
    username = session["user"]['username']
    record = __get_log_record.get(username, {'ip': '','time': 0})
    # 1分钟内不重复记录
    if int(time.time()) - record['time'] > 60 and request.remote == record['ip']:
        logger.write_log(username, request, '日志查看', '查看日志')
        __get_log_record[username] = {
            'ip': request.remote,
            'time': int(time.time()),
        }
    return web.json_response({ 'logs': logs[offset : count], 'total': logs_len })
