from database import DatabBase
from apscheduler.schedulers.asyncio import AsyncIOScheduler

class TimingTask:
    __db = DatabBase()
    __table_name = 'timing_task'
    __scheduler = None
    __client = None

    def __init__(self, mqtt_client):
        self.__client = mqtt_client

    def init_scheduler(self):
        self.__scheduler = AsyncIOScheduler()
        for task in self.__db.get_table(self.__table_name):
            self.__add_schedule_job(task)
        self.__scheduler.start()

    def __add_schedule_job(self, task):
        day_of_week = ",".join([str(i) for i in task['repeat']])
        time = task['time'].split(':')
        self.__scheduler.add_job(
                self.__client.set_device_list_state,
                'cron',
                day_of_week = day_of_week,
                hour = time[0],
                minute = time[1],
                args = [task['devices'], task['action']]
            )

    def get_task_list(self):
        return self.__db.get_table(self.__table_name)
    
    def add_task(self, task):
        if self.__db.check_item(self.__table_name, 'name', task['name']):
            return '定时任务名称重复'
        if task['name'] == '':
            return '定时任务名称不能为空'
        self.__db.add_item(self.__table_name, task)
        return 'OK'
    
    def delete_task(self, task):
        if not self.__db.check_item(self.__table_name, 'name', task['name']):
            return '定时任务名称不存在'
        self.__db.delete_item(self.__table_name, 'name', task['name'])
        return 'OK'
    
    def update_task(self, task):
        if not self.__db.check_item(self.__table_name, 'name', task['name']):
            return '定时任务名称不存在'
        self.__db.update_item(self.__table_name, 'name', task)
        return 'OK'
