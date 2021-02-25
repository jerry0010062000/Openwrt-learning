# network

-----

## basic 

幾乎所有的網路通訊是利用chunks而不是連續的flow來進行的，這些chunks被稱為packets，所以網路通訊是packet-based.
最初他們作為data存放在本機中，這些data需要從當前駐留的位置抵達網路上某個裝置的某個位置。
Data首先會被分解成packets然後在頭尾添加傳遞訊息，每當經過不同格式的設備時都會經歷這個過程。
當達到目的地後，刪除各種後期添加的頭尾資訊，再以正確的順序來還原原始的data

### Network interface 

Network interface 可以drop、forward、queue、delay、re-order packets
每個interface會有兩個queue，又稱為buffer，傳入數據包的queue稱為入口(ingress)，傳出的稱為出口(egress)
queue的大小可通過ifconfig來調整，當queue已滿時，任何傳入的packet將會被drop。ingress相較於egress的控制更為有限，只有forward或者drop。