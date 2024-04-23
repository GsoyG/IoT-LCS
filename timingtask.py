from database import DatabBase
from apscheduler.schedulers.asyncio import AsyncIOScheduler

class TimingTask:
    __db = DatabBase()
    __table_name = 'timing_task'
    __scheduler = None
    __client = None

    def __init__(self, mqtt_client):
        self.__client = mqtt_client

    # 初始化定时任务
    def init_scheduler(self):
        self.__scheduler = AsyncIOScheduler()
        for task in self.__db.get_table(self.__table_name):
            self.__add_schedule_job(task)
        self.__scheduler.start()

    # 添加定时任务事务
    def __add_schedule_job(self, task):
        if not task['enable']:
            return
        day_of_week = ",".join([str(i) for i in task['repeat']])
        time = task['time'].split(':')
        self.__scheduler.add_job(
                self.__client.set_device_list_state,
                'cron',
                day_of_week = day_of_week,
                hour = time[0],
                minute = time[1],
                args = [task['devices'], task['action']],
                id = task['name']
            )

    def get_task_list(self):
        return self.__db.get_table(self.__table_name)
    
    def add_task(self, task):
        name = task['name']
        if self.__db.check_item(self.__table_name, 'name', name):
            return '定时任务名称重复'
        if name == '':
            return '定时任务名称不能为空'
        self.__db.add_item(self.__table_name, task)
        self.__add_schedule_job(task)
        return 'OK'
    
    def delete_task(self, task):
        name = task['name']
        if not self.__db.check_item(self.__table_name, 'name', name):
            return '定时任务名称不存在'
        if self.__db.get_items(self.__table_name, 'name', name)[0]['enable']:
            self.__scheduler.remove_job(name)
        self.__db.delete_item(self.__table_name, 'name', name)
        return 'OK'
    
    def update_task(self, task):
        name = task['name']
        if not self.__db.check_item(self.__table_name, 'name', name):
            return '定时任务名称不存在'
        if self.__db.get_items(self.__table_name, 'name', name)[0]['enable']:
            self.__scheduler.remove_job(name)
        self.__db.update_item(self.__table_name, 'name', task)
        self.__add_schedule_job(self.__db.get_items(self.__table_name, 'name', name)[0])
        return 'OK'
