# ExTabbar
shell object test project


## todo

### Issue

1、分割窗口拖动位置时，文件列表窗口大小没有更新(fixed on 2019.10.12)
2、拖拽新标签到标签栏时，鼠标在最后位置，但是新建标签却在最前面的问题(fixed on 2019.09.15)
3、多个资源管理器窗口同时出现是全局变量 pExplorerWindow 重叠问题(fixed on 2019.09.19)
4、标签拖动到窗口边缘时，新窗口标题栏出现在屏幕之外

### Feature

1、跨窗口之间的标签拖放
2、支持 windows 10 1903的独立进程模式

### compile xconfig

It need XMLProfile class to compile xconfig. XMLProfile classes are open code from codeproject, and under the CPOL license. For some reason, we are not include the code in this Repository directly, you can find these codes on the codebase site, and the last section of this README has a way to find these codes.

###  XMLProfile calss(es)

<XMLProfile - a non-MFC, non-STL class to read and write XML profile files>, from

https://www.codeproject.com/Articles/27437/XMLProfile-a-non-MFC-non-STL-class-to-read-and-wri

is licensed under [The Code Project Open License (CPOL)](http://www.codeproject.com/info/cpol10.aspx)
