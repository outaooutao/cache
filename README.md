cache
=====
simulation.c 是在运行30秒后模拟用户查询的代码。为每个发生查询的秒设置了posix定时器。时间到了就在redis里查有此查询。

log_caculate.c  根据log，依次每个查询是否命中缓存。
