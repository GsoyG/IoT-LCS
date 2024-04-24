import json
from aiohttp import web
from services.devicelist import DeviceList

device: DeviceList = None
routes = web.RouteTableDef()

# 获取设备列表
@routes.get('/api/lighting/devices')
async def get_device_list(request):
    data = await device.get_device_list()
    if isinstance(data, str):
        return web.Response(status = 429, text = data)
    return web.json_response(data)

# 设置设备状态
@routes.get('/api/lighting/setState')
async def set_device_state(request):
    try: # 检查参数
        state_device = request.query['device']
        state = json.loads(request.query['state'])
    except KeyError:
        return web.Response(status = 400, text = '参数错误：找不到指定参数')
    
    # 设置设备状态
    data = await device.set_device_state_fast(state_device, state)
    if isinstance(data, str):
        return web.Response(status = 429, text = data)
    return web.json_response(data)
