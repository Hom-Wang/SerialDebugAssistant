[SerialDebugAssistant](https://github.com/Hom-Wang/SerialDebugAssistant)
========
* Author  : [Hom](https://github.com/Hom-Wang)
* Version : v0.1
* Update  : 2016/05/29

Description
========
SerialDebugAssistant 是一個串列調試助手，幫助微控制器的開發與除錯，透過自訂的傳輸封包格式，可以支援至多 24 筆資料輸入，初期版本先以各功能雛型實現為主。

預計實現的功能：
 > 基本串列傳輸的訊息顯示 → 基本已實現，但預設不開啟 (Main Window)  
 > 自訂義封包的串列傳輸示波器功能 → 基本已實現 (Tool - Serial Scope)  
 > 將串列示波器數據儲存，並透過 MATLAB 讀取與顯示(CTRL + S) → 基本已實現  
 > 將接收到的角度訊息顯示在螢幕上 → 尚未實現 (Tool - 3D Viewer)  
 > 透過 bin 檔與 IAP 功能來更新微控制器 → 尚未實現 (Tool - File Sender)  

View
========
<img src="https://lh3.googleusercontent.com/M9PTM55yv9suGe50KUyNmt4VCI4bKd5I173VWwcuRycOmeJ0MU0FhgLLoGC6U86zH1c6PRvK_XOGMRhWLXE7uWUfXNEkQYj2kGJJ8nLScLX3P8941J3f92h6SSxrPPz8A8w1c17EdnVIJJFiGqopzOmtUx-GIlHqjqCtUFzj0akXc4WNYXN23u-6a6fofp3lV7scEsQkoKFKU6yPhF9rYIOiwdqdaF4EYyG3W67LsB86oxgUlkWy9lF24uZeKLyIk5e0SOWu7mFvjT8jk69KqdpdRL0bHe8RWS90xx8hPjR26DFPvoUMvOmzk-terMOoeK2wLpMlal3Swbhcr7QZejKNgLrrEZnb4VJ9LZvbcWY0KISiMVoC82F37c9yIExnAXHbkMzLz6H6Txuk49ds2gugOTo5J57I8JgMLnWkongk0DUZpI_-AJcO-rFyUoM_vb-uMzhoUNXi2lZTn9VrnHMEx_1JyK5_JVtCDOeCtwG5bBKozirs8pYzlcLTnlh1wExqv9BvByebMN3TymHZURr7j1gnmT-SoYMDgAtIP7T6HQdzvOECqMzxo2QgNd0QF6KpjC0RSxUvwZAzSoYTyZth5LNkByV1=w892-h604-no" />

更多圖片 [Google+ albums](https://goo.gl/photos/eMeMyJ5LZUsToNJP8)

Packet Format
========
<img src="https://lh3.googleusercontent.com/N9nRQTGRXDteKrLncUyIdKaGctyJDWujKP46JiKsgOTTlqcWKbu5X_y219D1T_nmcKDLsSgfl3VqNB2s1DCw_XvywzrJRe5dJe6SMTGQOrvcIg_yaM7IgUQvbSTlQEBpgPQoK1E4OeoUoTKINUfuzzqmk4DM0HsvM4gV5VVrPNzixmA3mZ2b_S0Og_mMd6FgazNf3wyEFWS8jS5FSss4uytwZkgjAptq_u3xDTo3n-GEGbrleD3offCt2Id6Yop3AHgYlHY149AvH1twy5YCF9wOhdpColqGtTaC2SjLyO0VD9qWhX55uYDBPx91Y0n2XYl-TAyNE0iymVMUfM8N7y8h9cf_nVTmWRhcMnwRUYRONmX1YgDTKvelnV1pgokMYiIsubCRN9-VvUVZyon_F2PjQ9vkIMLG1uESueC9uFSvp8J8csSW6-pZd-qe4bVp4TkFf0RJCTw8vOi7Km5ajjhgQ-D8SXXeITDeayu0So3jIGsZd8oV6ObQa3YsVc4Unx67tThacV741MZuu1sdSQKvnVi9r1DsPnlvC-PBGuuW2BFf5lauJreUv6B-cKeGw5yj6JVeZhKYCtXd3YvkD_-xqlvmP1SD=w1101-h257-no" />

