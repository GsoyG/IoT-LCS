import time
from tinydb import where
from services.database import DatabBase

__db = DatabBase('logger')

def write_log(user, request, type, message):
    address = request.headers.get("X-Forwarded-For", '')
    if address == '':
        address = request.headers.get("X-Real-IP", request.remote)

    timestamp = int(time.time())
    __db.add_item({
        'user': user,
        'address': address,
        'type': type,
        'message': message,
        'timestamp': timestamp
    })
    current_time = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(timestamp))
    print(f'LOG:[{current_time}] {user} {address} {type} {message}')

# 查询日志，参数过滤日志
def get_logs(start_time = None, end_time = None, user = None, address = None, type = None, message = None):
    start_query = int(start_time) if (start_time is not None) and (start_time != '') else 0
    end_query = int(end_time) if (end_time is not None) and (end_time != '') else time.time()
    user_query = user if user is not None else ''
    address_query = address if address is not None else ''
    type_query = type if type is not None else ''
    message_query = message if message is not None else ''
    
    logs = __db.get_items_query(
        (where('timestamp') >= start_query) &
        (where('timestamp') <= end_query) &
        (where('user').search(user_query)) &
        (where('address').search(address_query)) &
        (where('type').search(type_query)) &
        (where('message').search(message_query))
    )
    logs.reverse()
    return logs
