import json
import services.logger as logger
from services.database import DatabBase
from aiohttp import web
from aiohttp_session import new_session, get_session

routes = web.RouteTableDef()

__db = DatabBase('user_info')

def __check_user(username, password):
    user = __db.get_items('username', username)[0]
    if user['password'] == password: return True
    return False

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

    # 检查用户名和密码
    if __check_user(username, password):
        session = await new_session(request)
        session['user'] = { 'username': username }

        logger.write_log(username, request, '用户认证', '成功登入')
        return web.json_response({ 'status': 'OK' })
    else:
        logger.write_log(username, request, '用户认证', '登入失败，用户名或密码错误')
        return web.Response(status = 401, text = '用户名或密码错误')

# 登出
@routes.post('/api/logout')
async def logout(request):
    session = await get_session(request)
    logger.write_log(session["user"]['username'], request, '用户认证', '成功登出')
    
    if "user" in session:
        del session["user"]
    return web.json_response({ 'status': 'OK' })

# 获取用户信息
@routes.get('/api/user/info')
async def get_user_info(request):
    session = await get_session(request)
    user = __db.get_items('username', session["user"]['username'])[0]

    return web.json_response({
        'username': user['username'],
        'qq': user['qq'],
        'email': user['email']
    })
