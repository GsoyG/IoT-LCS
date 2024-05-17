import time
from tinydb import where
from services.database import DatabBase

__db = DatabBase('logger')

def write_log(user, message):
    __db.add_item({
        'user': user,
        'message': message,
        'time': time.time()
    })

def get_logs(start_time = None, end_time = None, user = None, message = None):
    logs = __db.get_table()
    return logs
