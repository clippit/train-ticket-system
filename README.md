# 概述

本程序实现了服务器/客户端结构的铁路售票系统模拟。有两种交互模式：

* 基于本地 FIFO(命名管道)
* 基于网络 Socket

两种模式可以通过启动参数进行切换（详见下文）。

## 服务端

服务端采用守护进程模式工作。为简化实现，没有外部配置文件，没有管理脚本。在命令行中可以直接启动服务器，关闭则可使用`killall`命令。

服务器启动后会监听用户请求，一旦接收到便会`fork`开启一个新的子进程处理，并将结果返回给相应客户端。

服务器日志使用`syslog`发送，系统需要有`syslogd`或`syslog-ng`等日志管理守护进程运行。在大多数发行版中，它是预配置好的，服务端的日志一般可以在`/var/log/messages.log`中看到。

服务端使用了 [SQLite 3](http://www.sqlite.org/) 数据库，系统中需存在libsqlite3库。一般可以使用发行版相应的软件包管理器进行安装，例如在 [Arch Linux](http://www.archlinux.org/) 下可以使用以下命令：

    sudo pacman -S sqlite

服务端在处理数据库查询时使用了参数绑定，可以防止数据库注入攻击。

## 客户端

客户端采用纯命令行实现。当没有在启动参数中指定用户名和密码时，程序会提示用户输入（如果是注册操作，会提示输入两遍密码以防止输错）。此外所有操作均在命令行参数中完成，没有额外的交互界面。

程序实现了用户注册，登录后查询班次、购买车票、查看已购车票、退票的功能。详细使用方法见下文。


# 编译安装

* 克隆版本库

        $ git clone git://github.com/clippit/train-ticket-system.git
        $ cd train-ticket-system

* 编译

        $ make

* 导入数据库和预设数据

        $ sqlite3 data.sqlite < dummy.sql

本程序在 Arch Linux, gcc 4.7.0, glibc 2.15, sqlite 3.7.11 下测试编译通过。

# 使用

## 服务端

服务端采用守护进程模式运行，参数有：

    -m, --offline 离线模式，使用FIFO，默认启动模式
    -M, --online  在线模式，使用Socket

关闭服务器，可以使用`killall ticketd`。

## 客户端

客户端命令行用法：

    ticket [MODE] [-u USERNAME] [-p PASSWORD] [-r|-s|-o|-v|-f] [OPTIONS ...]

运行模式：

    -m, --offline           离线模式，使用FIFO，默认启动模式
    -M, --online HOSTNAME   在线模式，使用Socket，需要指定连接的主机名

可以在参数中使用如下参数指定登录信息：

    -u, --username USERNAME
    -p, --password PASSWORD

如果没有指定这两个参数，程序将提示用户输入（输入密码时屏幕不回显）。如果指定了注册操作，程序会要求用户输入两遍密码，以防止输错。

客户端的5种操作（登录、搜索、订购、查看、退款），除了搜索和订购不能同时执行（因为共享启动参数名）以外，可以依次执行。例如通过一条命令可以完成注册、订购车票、查看自己所有订单这三个操作。具体用法请参考`ticket --help`。


# 自动测试脚本

附带脚本 `audo_test.sh` 可以一次性开启15个客户端，连接服务器进行不同的操作，以测试并发处理能力。该脚本会删除现有的数据库并使用 `dummy.sql` 重新创建。不带参数运行 `./auto_test.sh` 以测试FIFO性能，运行 `./auto_test.sh online` 以测试Socket性能。


# 实现

## 代码组织

程序入口分别在`server.c`和`client.c`中，这两个文件主要进行参数解析的工作，并注册相应的运行模式。两种运行模式分别定义了一些公共接口，使用函数指针结构指向具体的函数。完成后程序交给Controller部分处理（该部分源码在controller目录下）。不同的运行模式有着不同的数据传输方式，这些在相应的`*_fifo.c`和`*_socket.c`中实现。服务端的数据库操作在`cserver.c`中实现。此外，公共的数据定义在model目录下。

## 数据传输

程序定义了`Request`和`Response`两个struct作为请求和回应的载荷。定义如下：

    +-----------------------------------+
    | request_t                         |
    |-----------------------------------|
    | client_pid   客户端PID            |
    | action       客户端动作           |
    | username     用户名               |
    | password     密码                 |
    | name         火车班次(搜索和订购) |
    | from         出发地(搜索)         |
    | to           目的地(搜索)         |
    | order_id     订单号(退款)         |
    | amount       数量(订购)           |
    +-----------------------------------+

    +-----------------------------------+
    | response_t                        |
    |-----------------------------------|
    | code         返回码               |
    | content      返回内容             |
    +-----------------------------------+
    
## 数据库结构

详见`dummy.sql`文件。

