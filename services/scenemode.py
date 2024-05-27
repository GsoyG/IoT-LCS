from services.database import DatabBase

class SceneMode:
    __db = DatabBase('scene_mode')
    __client = None

    def __init__(self, mqtt_client):
        self.__client = mqtt_client
    
    def get_mode_list(self):
        return self.__db.get_table()
    
    def switch_mode(self, name):
        if not self.__db.check_item('name', name):
            return '模式名称不存在'
        
        mode = self.__db.get_items('name', name)[0]
        enable_modes = self.__db.get_items('enable', True)
        # 检查是否有开启的模式
        if len(enable_modes) == 1:
            enable_mode = enable_modes[0]

            # 检查是否为关闭模式
            if enable_mode['name'] == mode['name']:
                self.__db.update_item('name', { 'name': name, 'enable': False })
                return 'OK'
            # 切换模式前关闭当前模式
            self.__db.update_item('name', { 'name': enable_mode['name'], 'enable': False })
        
        # 开启模式
        self.__db.update_item('name', { 'name': name, 'enable': True })
        self.__client.set_device_list_state(mode['devices'], mode['action'])
        return 'OK'

    def add_mode(self, mode):
        # 检查参数
        name = mode['name']
        if self.__db.check_item('name', name):
            return '模式名称重复'
        if name == '':
            return '模式名称不能为空'
        
        # 添加模式
        self.__db.add_item(mode)
        return 'OK'
    
    def delete_mode(self, mode):
        # 检查参数
        name = mode['name']
        if not self.__db.check_item('name', name):
            return '模式名称不存在'
        
        self.__db.delete_item('name', name)
        return 'OK'
    
    def update_mode(self, mode):
        # 检查参数
        name = mode['name']
        if not self.__db.check_item('name', name):
            return '模式名称不存在'
        
        # 检查模式是否已经启动
        if self.__db.get_items('name', name)[0]['enable']:
            self.__client.set_device_list_state(mode['devices'], mode['action'])
        self.__db.update_item('name', mode)
        return 'OK'
    