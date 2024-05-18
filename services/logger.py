import time
from tinydb import where
from services.database import DatabBase

__db = DatabBase('logger')

def write_log(user, message):
    __db.add_item({
        'user': user,
        'message': message,
        'timestamp': int(time.time())
    })

# 查询日志，参数过滤日志
def get_logs(start_time = None, end_time = None, user = None, message = None):
    start_query = start_time if start_time is not None else 0
    end_query = end_time if end_time is not None else time.time()
    user_query = user if user is not None else ''
    message_query = message if message is not None else ''
    logs = __db.get_items_query(
        (where('timestamp') >= start_query) &
        (where('timestamp') <= end_query) &
        (where('user').search(user_query)) &
        (where('message').search(message_query))
    )
    return logs
