[SerialDebugAssistant](https://github.com/Hom-Wang/SerialDebugAssistant)
========
* Author  : [Hom](https://github.com/Hom-Wang)
* Version : v0.1
* Update  : 2016/05/29

Description
========
SerialDebugAssistant 是一個串列調試助手，幫助微控制器的開發與除錯，透過自訂的傳輸封包格式，可以支援至多 24 筆資料輸入，初期版本先以各功能雛型實現為主。

預計實現的功能：
 > 1. 基本串列傳輸的訊息顯示 → 基本已實現，但預設不開啟 (Main Window)  
 > 2. 自訂義封包的串列傳輸示波器功能 → 基本已實現 (Tool - Serial Scope)  
 > 3. 將串列示波器數據儲存，並透過 MATLAB 讀取與顯示(CTRL + S) → 基本已實現  
 > 4. 將接收到的角度訊息顯示在螢幕上 → 尚未實現 (Tool - 3D Viewer)  
 > 5. 透過 bin 檔與 IAP 功能來更新微控制器 → 尚未實現 (Tool - File Sender)  

View
========
<img src="https://lh3.googleusercontent.com/XQv14GvwBFKAHbIAOj8KPBsJ43yr9p8uTNn9QrNmpgDvhHbvkX94nJ5TcriPcta8ViGYZBf8TL-VxY-7AfiwAy2QE6mI9R8TRAGDnk5JdguSxfT3noMxf5JyRixoIONIug7qRquH7dHdkiDWke_olBfszOiEoIzxq1_01A4BVAL2goB-JhhDILr7L3b28PLH3T74fIzG-Yrv8kWJ02j2lA-i5Xi48GNoURSSlRShFQKPmi1mKJ8rIvWAjcuiSRA0vVwaH8I0zoEHOoC3Joc2agLb84AcLLhatVReqzwkQu28QfADVf0NPzc1Ta2-suyX7JDoZfwmaPVwghS6SaPKmySXk5NKiFnYBqiv03EeSYqyyPXFoCN2TkRtElKR-0zn6O0u9odeJyl7mcicSpp3kSGfc-R19H3L8BYVq5LCa0Z0_Mm4Wzmio8l3DUOdX_vryQkPFIriDezM1S0D0orh1DS9q7kE7lJgXK2qlRL38kBNtTudFKIdoCtuJh45c5NBEOkYHm3cMrYhPYyDBR-7udezuD6J5hCUgdzUmdd2r53Q7qMs_QypILmGNXpWir7t-_CDwavWlHSIwXdsUL0wQBVPTzWi-l0P=w785-h518-no" />

更多圖片 [Google+ albums](https://goo.gl/photos/eMeMyJ5LZUsToNJP8)

Packet Format
========
<img src="https://lh3.googleusercontent.com/N9nRQTGRXDteKrLncUyIdKaGctyJDWujKP46JiKsgOTTlqcWKbu5X_y219D1T_nmcKDLsSgfl3VqNB2s1DCw_XvywzrJRe5dJe6SMTGQOrvcIg_yaM7IgUQvbSTlQEBpgPQoK1E4OeoUoTKINUfuzzqmk4DM0HsvM4gV5VVrPNzixmA3mZ2b_S0Og_mMd6FgazNf3wyEFWS8jS5FSss4uytwZkgjAptq_u3xDTo3n-GEGbrleD3offCt2Id6Yop3AHgYlHY149AvH1twy5YCF9wOhdpColqGtTaC2SjLyO0VD9qWhX55uYDBPx91Y0n2XYl-TAyNE0iymVMUfM8N7y8h9cf_nVTmWRhcMnwRUYRONmX1YgDTKvelnV1pgokMYiIsubCRN9-VvUVZyon_F2PjQ9vkIMLG1uESueC9uFSvp8J8csSW6-pZd-qe4bVp4TkFf0RJCTw8vOi7Km5ajjhgQ-D8SXXeITDeayu0So3jIGsZd8oV6ObQa3YsVc4Unx67tThacV741MZuu1sdSQKvnVi9r1DsPnlvC-PBGuuW2BFf5lauJreUv6B-cKeGw5yj6JVeZhKYCtXd3YvkD_-xqlvmP1SD=w1101-h257-no" />

Release
========
[Google drive](https://drive.google.com/open?id=0BzL2wwAot6oPWG1DWFNmN1Y5WlE)
