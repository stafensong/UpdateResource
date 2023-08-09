# UpdateResource
UpdateResource为PE资源替换工具, 可以将离线安装器的内置包替换为新的发布包，并替换相关字符串资源
## 准备
- 离线安装器: 如：install.exe
- 发布包: 如: Release.zip

## 使用说明
- 查看字符串，string_key为字符串资源ID
```
   > UpdateResource.exe -c show -f install.exe -t string -k string_key
```
- 更新字符串
```
   > UpdateResource.exe -c update -f install.exe -t string -k string_key -v string_value
```
- 更新发布包
```
   > UpdateResource.exe -c update -f install.exe -t exeres -k res_key -v Release.zip
```
