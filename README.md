# bk7231_freertos_full_code



## 当前repository中submodule所在分支

| submodule | branch |
| --------- | ------ |
| beken378  | master |

> clone代码后，需执行如下命令：

```shell
cd beken378
git checkout master
```

> 如果不确定submodule所在分支，可用如下命令查找并手动确认

```shell
cd beken378
git branch -r --contains CommitID
```



## 代码管理策略 

随着项目的日益增多，决定使用`submodule+branch`的方式来管理
1. 每类项目单独创建一个`repository`
2. 同类项目用分支区分不同产品、不同代理商、不同客户
3. `beken378`使用`submodule`关联，`beken378`包含协议栈源码，相当于`fullcode`，发布时可使用脚本自动剔除协议栈等源码
4. 在项目稳定前，可直接使用`submodule`主分支，如果`submodule`有重大功能合入，可提前创建项目分支
5. 在提交代码时，不同目的的修改分开提交，便于向其它分支合入
6. 在提交代码后，如果需要将修改合入其它分支，使用`git cherry-pick commitid`
7. 在版本发布后，使用`git tag -a -m "notes" tag_name`创建标签，并用`git push --tags`提交



## submodule使用说明

### 创建submodule
1. `git submodule add http://192.168.0.6/wifi/beken_wifi_sdk.git beken378`
2. 修改`.gitmodules`中的`url`为相对路径，便于`ssh`和`http`同时使用
3. `git add .gitmodules beken378`
4. `git commit -m "xxx"`

注：
1. 所有`submodules`可能使用不同的分支，建议使用如下命令，指定所有`submodules`（包括主`repository`）的分支
`git branch --set-upstream-to=origin/xxx master`



### 获取submodule

> 如果使用`http`而不是`ssh`获取代码，请先运行如下命令(将用户名密码保存到本地文件中，解决`submodule`获取时无法访问的问题)
>
> `git config --global credential.helper store`



1. `git clone --recurse-submodules http://xxx.git`
    或者
2. `git clone http://xxx.git`
3. `cd xxx`
4. `git submodule update --init --recursive`



### 更新submodule
可以在`repository`根目录使用`git pull --recurse-submodules`命令更新整个工程
当然也可以进入特定submodule通过`git pull`单独更新

推荐使用前者更新，因为更新后`submodules`仍然停留在主`repository`关联的`commit`，不会主动切换到最新`commit`



### 提交submodule

需要先在`submodule`中提交修改，然后在主`repository`中更新`submodule`
1. `cd beken378` (the submodle name)
2. `git add xxx`
3. `git commit -m ""`
4. `git push origin xxx:yyy`
5. cd .. (the root of repo)
6. `git add beken378`
7. `git commit -m ""`
8. `git push origin`



## Build with Makefile

- Install arm-gcc tool-chain and add path to system environment
- update `make.exe` in rt-thread env with `tool\make\make.exe` if you want to use parallel compiling with it
- update `beken378\app\config\sys_config.h` if needed
- make target [-j6]

```shell
make bk7231u -j8
make bk7251
make bk7231 -j6
make ip
make ble
make clean
```



## 版本发布 

在发布版本时，`wifi/ble stack`中的代码编译以库的形式打包，然后对代码打标签并发布，方法如下

- 根据项目实际情况修改`sys_config.h`中的`CFG_SOC_NAME`，并返回项目根目录

编译库文件
- `scons --buildlib=beken_ip` 编译结束后会根据`CFG_SOC_NAME`重命名为`beken378/ip/libip72xx.a`
- `scons --buildlib=beken_ble` 编译结束后会重命名为`beken378/driver/ble/libble.a`

编译好库并提交代码以后，需要打标签，由于`submodule`本身就是以`commit id`的形式存在，无需单独打标签。
（当然，如果为了方便查找，可以用相同名称在相应代码库中打标签。）

- `git tag -a -m "" tag_name`
- `git push --tags`

运行脚本打包文件并发布
- run `release_tool.bat`
