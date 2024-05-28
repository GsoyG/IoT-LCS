<template>
	<v-list>
		<v-list-item prepend-avatar="http://q2.qlogo.cn/headimg_dl?dst_uin=2754205302&spec=100" title="作者：Gsoy"
			subtitle="日期：2024年4月1日"></v-list-item>
	</v-list>
	<v-divider></v-divider>
	<v-list nav>
		<v-list-item prepend-icon="mdi-lightbulb-group" title="设备管理" value="device" href="#/"></v-list-item>
		<v-list-item prepend-icon="mdi-calendar-clock" title="定时任务" value="timing" href="#/timing"></v-list-item>
		<v-list-item prepend-icon="mdi-view-dashboard" title="情景模式" value="scene" href="#/scene"></v-list-item>
		<v-list-item prepend-icon="mdi-account-box-multiple" title="账户管理" value="account" href="#/account"></v-list-item>
		<v-list-item prepend-icon="mdi-text-box-search" title="日志查看" value="log" href="#/log"></v-list-item>
		<v-list-item prepend-icon="mdi-cog" title="系统设置" value="setting" href="#/setting"></v-list-item>
	</v-list>
</template>
