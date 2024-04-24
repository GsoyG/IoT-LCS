import json
from aiohttp import web
from aiohttp_session import new_session, get_session

routes = web.RouteTableDef()

# 登入
@routes.post('/api/login')
async def login(request):
    try: # 检查参数
        data = await request.json()
    except json.decoder.JSONDecodeError:
        return web.Response(status = 400, text = '参数错误：参数格式错误')
    
    username = data.get('username')
    password = data.get('password')
    if not (username and password):
        return web.Response(status = 400, text = '参数错误：缺少用户名或密码')

    if username == 'admin' and password == 'password':
        session = await new_session(request)
        session['user'] = { 'username': username }
        return web.json_response({ 'status': 'OK' })
    else: return web.Response(status = 401, text = '用户名或密码错误')

# 登出
@routes.post('/api/logout')
async def logout(request):
    session = await get_session(request)
    if "user" in session:
        del session["user"]
    return web.json_response({ 'status': 'OK' })
