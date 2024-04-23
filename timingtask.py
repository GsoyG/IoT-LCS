from database import DatabBase

class TimingTask:
    __db = DatabBase()
    __table_name = 'timing_task'

    def getTaskList(self):
        return self.__db.getTable(self.__table_name)
    
    def addTask(self, task):
        if self.__db.checkItem(self.__table_name, 'name', task['name']):
            return '定时任务名称重复'
        if task['name'] == '':
            return '定时任务名称不能为空'
        self.__db.addItem(self.__table_name, task)
        return 'OK'
    
    def deleteTask(self, task):
        if not self.__db.checkItem(self.__table_name, 'name', task['name']):
            return '定时任务名称不存在'
        self.__db.deleteItem(self.__table_name, 'name', task['name'])
        return 'OK'
    
    def updateTask(self, task):
        if not self.__db.checkItem(self.__table_name, 'name', task['name']):
            return '定时任务名称不存在'
        self.__db.updateItem(self.__table_name, 'name', task)
        return 'OK'
