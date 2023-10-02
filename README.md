# Tcp-ip-windows-miltithread-server-and-client
目前Server能

1. 擁有一個控制台
   
1.1 用戶功能 : search_client_by_number [i]
   
1.2 用戶功能 : traversal_client	

1.3 用戶功能 : kick [i]			

1.4 用戶功能 : server_close			


2. 擁有一個子線程用作接收客戶端

3. 擁有 [客戶端數量] 個子線程用作客戶端Recv
   
3.1 處理客戶端信息 : personal_information
   
3.2 處理客戶端信息 : client_close


目前Client能
1. 擁有一個控制台
   
1.1 用戶功能 : personal_information
   
1.2 用戶功能 : client_close


2. 擁有一個子線程用作接收伺服器Recv
   
2.1 處理伺服器信息 : kick
