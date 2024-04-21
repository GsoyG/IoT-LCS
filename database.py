from tinydb import TinyDB, Query

class DatabBase:
    __db = None
    __db_path = 'database.json'

    def __init__(self):
        self.__db = TinyDB(self.__db_path)

    def getTable(self, table_name):
        table = self.__db.table(table_name)
        return table.all()
    
    def addItem(self, table_name, data):
        table = self.__db.table(table_name)
        return table.insert(data)
    
    def updateItem(self, table_name, idKey, data):
        table = self.__db.table(table_name)
        return table.update(data, Query()[idKey] == data[idKey])
    
    def deleteItem(self, table_name, idKey, idValue):
        table = self.__db.table(table_name)
        return table.remove(Query()[idKey] == idValue)
    
    # 检查表项是否存在
    def checkItem(self, table_name, idKey, idValue):
        table = self.__db.table(table_name)
        return table.contains(Query()[idKey] == idValue)
