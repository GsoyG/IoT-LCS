from tinydb import TinyDB, Query

class DatabBase:
    __db = None
    __db_path = 'database.json'

    def __init__(self):
        self.__db = TinyDB(self.__db_path)

    def get_table(self, table_name):
        table = self.__db.table(table_name)
        return table.all()
    
    def get_items(self, table_name, id_key, id_value):
        table = self.__db.table(table_name)
        return table.search(Query()[id_key] == id_value)
    
    def add_item(self, table_name, data):
        table = self.__db.table(table_name)
        return table.insert(data)
    
    def update_item(self, table_name, id_key, data):
        table = self.__db.table(table_name)
        return table.update(data, Query()[id_key] == data[id_key])
    
    def delete_item(self, table_name, id_key, id_value):
        table = self.__db.table(table_name)
        return table.remove(Query()[id_key] == id_value)
    
    # 检查表项是否存在
    def check_item(self, table_name, id_key, id_value):
        table = self.__db.table(table_name)
        return table.contains(Query()[id_key] == id_value)
