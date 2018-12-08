# yolo-people-counting

<br>
基于yolo的人流/人数识别系统

yujianke.c是在demo.c的基础上改写的

detector.c中添加了yujianke.c的启动指令

start.c生成用于启动的可执行文件

stop.c生成用于控制该系统的可执行文件
<br>
<br>
yolo源代码和使用方法参照yolo官网：
https://pjreddie.com/darknet/yolo/

<br>
image.c中添加了对中文标签的支持。相应的中文标签在data/coco_cn.names中

coco.names和coco_cn.names已改成voc.names和其相应的20个中文标签。原始的coco保存于副件中

标签添改可使用/data/labels/*.py。其中makeLabels.py为生成原版coco_cn.names的80个标签

因为只需要显示标签“人”（通过修改cfg/coco.data中第一行的classes=1实现），所以使用时并没修改其他标签。
<br>
该方法来自：
https://github.com/PaulChongPeng/darknet/commit/798fe7cc4176d452a83d63eb261d6129e397a521
<br>
<br>
<br>

### Mark.py:
用于批量生成标记文件。因为生成的标记位置相同，所以在获取照片时尽量保证人脸的位置不变，且背景较为单一。
使用方法（Linux）：

1.使用labelimg生成第0张照片的标记文件（可适当扩大标记位置）

2.在Annotations中生成相应数量的文件（以1747份照片为例）
for i in {1..1746}; do touch ${i}.xml;done;

3.根据第0份标记信息，修改Mark.py

4.python Mark.py

5.修改标记文件名称
i=1000001; for f in *.xml; do mv "$f" 181107_${i#1}.xml; ((i++));done<br>

## 关于训练：
### 1.新建文件夹：
<br>
VOCdevkit<br>
+VOC20XX<br>
++JPEGImages<br>
++Annotations<br>
++ImageSets<br>
+++Main<br>
<br>
JPEGImages中存放照片，Annotations中存放标记文件（可以使用labelimg标记），Main中存放train.txt（训练文件的文件名），val.txt（验证文件的文件名），trainval.txt（train+val）,test.txt（测试文件的文件名，非必须）
<br>
<br>

### 2.使用voc_txt.c:
一个得到上述txt文件的一个demo。注意使用时修改相关内容。生成的内容会带有路径信息和后缀，需要手动去除。（文本替换）
<br>
<br>

### 3.使用voc_label.py:
通过上述txt文件自动生成文件路径的程序。注意使用时修改相关内容。（注意jpg和jpeg）.
<br>
注意：应把生成的20xx_train.txt和20xx_val.txt合并。<br>
cat 20xx_train.txt 20xx_val.txt >train.txt
<br>
<br>

### 4.修改参数：
#### cfg/voc.data：修改classes数量、路径等
classes= 1 #classes为训练样本集的类别总数
train = /home/user/darknet/train.txt #train的路径为训练样本集所在的路径 
valid = /home/user/darknet/2007_val.txt #valid的路径为验证样本集所在的路径 
names = data/voc.names #names的路径为data/voc.names文件所在的路径 
backup = backup #务必确保权重文件输出地址存在。默认在darknet根目录下
#### data/voc.names:修改标签名
#### cfg/yolov3-voc.cfg：修改三处[yolo]前后文中的classes和filters=（3*（classes+5））。注意最前面的Testing部分要注释掉，Training部分的解注。bath和subdivisions根据显存大小进行修改（越大越快，也越容易爆显存。TITAN XP上，选择了32，16）
<br>
<br>

### 5.开始训练：
nohup ./darknet detector train cfg/voc.data cfg/yolov3-voc.cfg darknet53.conv.74 -gpus 0,1 &<br>
该指令将会将训练任务放在后台。打开darknet文件夹下的nohup.out可以查看日志。
<br>
<br>

### 特别注意：
在源码src/data.c中，fill_truth_detection函数是用于利用图片路径获取labels路径。它会将“JPEGImages”换成“labels”，“jpg”换成“txt”，“JPEG”换成“txt”。如果原本的文件名后缀为小写的“jpeg”，注意修改源码
<br>

## 训练中
### 每秒刷新显卡监视窗口：
watch -n 1 nvidia-smi
### 刷新查看日志文件nohup.out:
tail -f nohup.out

