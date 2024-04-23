from database import DatabBase

class TimingTask:
    __db = DatabBase()
    __table_name = 'timing_task'

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
