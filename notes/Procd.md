# Procd
===================================


* 主要功能
  * 開機時執行初始化並取代成為pid = 1
  * [檢查config是否變化]
  * 維護process、接收request、增減process
  * [procd.sh](#procd_sh)



<h2 id="proce_sh">Procd.sh</h2>
由於使用ubus method必須使用json格式，容易出錯，procd.sh將其封裝成函數

```bash
procd_open_trigger				#開始定義一組trigger
procd_close_trigger				#結束定義trigger
procd_add_reload_trigger		#定義關注的config(位於/etc/config下)，當
								#reload_config後如果關注的檔案被修改則觸
								#reload_service()
```

```bash
procd_open_instance				#開始新增一個instance
procd_set_param					#設定instance參數
	-command	@啟動的命令
	-respawn	@意外結束時的重啟機制
	-env		@環境變數
	-file		@比較此文件是否有改變
	-netdev		@綁定的網路設備
	-limits		@process的資源限制
procd_close_intsance			#結束定義instance
```



