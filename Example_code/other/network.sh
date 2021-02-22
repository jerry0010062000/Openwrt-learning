#!/bin/sh /etc/rc.common

START=20
STOP=90

USE_PROCD=1  # 表示由 procd 进程管理
             # 更多说明:https://openwrt.org/start?id=docs/guide-developer/procd-init-scripts

init_switch() {
        setup_switch() { return 0; }

        include /lib/network  
        setup_switch    # 调用的是 /lib/network 里定义的 setup_switch
}

start_service() {
        init_switch                                     
        #初始化 switch
        procd_open_instance
        # 创建一个实例
        # ubus call service list 可以查看实例
        procd_set_param command /sbin/netifd
        # 执行的命令是 /sbin/netifd
        procd_set_param respawn
        # 定义respawn参数，告知procd当程序退出后尝试进行重启
        procd_set_param watch network.interface
        # 监控的ubs 名字
        [ -e /proc/sys/kernel/core_pattern ] && {
                procd_set_param limits core="unlimited"
                echo '/tmp/%e.%p.%s.%t.core' > /proc/sys/kernel/core_pattern
        }
        procd_close_instance
        # 关闭一个实例
}

reload_service() {
        init_switch
        ubus call network reload
        /sbin/wifi reload_legacy
}