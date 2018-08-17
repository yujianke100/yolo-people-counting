# yolo-people-counting
基于yolo的人流/人数识别系统
yujianke.c是在demo.c的基础上改写的
detector.c中添加了yujianke.c的启动指令
start.c生成用于启动的可执行文件
stop.c生成用于控制该系统的可执行文件

yolo源代码和使用方法参照yolo官网：
https://pjreddie.com/darknet/yolo/

image.c中添加了对中文标签的支持。相应的中文标签在data/coco_cn.names中
coco.names和coco_cn.names已改成voc.names和其相应的20个中文标签。原始的coco保存于副件中
标签添改可使用/data/labels/*.py。其中makeLabels.py为生成原版coco_cn.names的80个标签
因为只需要显示标签“人”（通过修改cfg/coco.data中第一行的classes=1实现），所以使用时并没修改其他标签。
该方法来自：
https://github.com/PaulChongPeng/darknet/commit/798fe7cc4176d452a83d63eb261d6129e397a521
