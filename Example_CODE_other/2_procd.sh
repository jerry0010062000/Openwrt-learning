#!/bin/sh /etc/rc.common
START=93	#start order
USE_PROCD=1		#using procd

start(){
	/bin/sh "/root/normal_start.sh"
}

start_service() {
	#procd.sh
    procd_open_instance 
    procd_set_param command /bin/sh "/root/oops.sh" 
    procd_set_param respawn
    procd_set_param stdout 1
    procd_set_param stderr 1 				
    procd_close_instance
}

service_triggers(){
	procd_add_reload_trigger "dhcp"
}

reload_service(){
	logger -t LOG_INFO "Detect config change, Process reloading..."
	restart
}

stop(){
	logger -t LOG_INFO "Process stopped!"
}

restart(){
	stop
	start
}