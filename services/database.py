from tinydb import TinyDB, Query

class DatabBase:
    __db = None
    __db_path = 'database.json'
    __table_name = ''

    def __init__(self, table_name):
        self.__db = TinyDB(self.__db_path)
        self.__table_name = table_name

    def get_table(self):
        table = self.__db.table(self.__table_name)
        return table.all()
    
    def get_items(self, id_key, id_value):
        table = self.__db.table(self.__table_name)
        return table.search(Query()[id_key] == id_value)
    
    def add_item(self, data):
        table = self.__db.table(self.__table_name)
        return table.insert(data)
    
    def update_item(self, id_key, data):
        table = self.__db.table(self.__table_name)
        return table.update(data, Query()[id_key] == data[id_key])
    
    def delete_item(self, id_key, id_value):
        table = self.__db.table(self.__table_name)
        return table.remove(Query()[id_key] == id_value)
    
    # 检查表项是否存在
    def check_item(self, id_key, id_value):
        table = self.__db.table(self.__table_name)
        return table.contains(Query()[id_key] == id_value)
