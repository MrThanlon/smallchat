# SmallChat

> 一个使用[libhttp-ev](https://github.com/MrThanlon/libhttp-ev)的例子。

## 接口定义

### 查询聊天室

GET `/room`

### 加入房间并接收消息

websocket `/message`

连接后发送你的用户名和要加入的房间号，如果没被关闭就是成功了，之后要发消息就直接发送即可。