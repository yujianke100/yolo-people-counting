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

### Mark.py（旧方案）:
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
## -----------------新方案-----------------
训练依旧需要JPEGimages文件夹和labels文件夹

https://github.com/xhuvom/darknetFaceID

利用yolo的人脸识别，做到一个摄像头，实时保存人脸图像和坐标信息。
具体操作步骤可见readme

### 采样部分：
要点1：使用QuanHua提供的人脸权重文件

要点2：opencv要为2版本

要点3：修改image.c中的223行和227行

要点4：源码本身有问题，需要在save_image_jpg函数中的sprintf改成sprintf(buff, "%s%d.jpg", name, imnumber);（或在使用时不加‘_’），并在227行后加上imnumber++;

要点5：运行前保证223行和227行中定位的文件夹存在。

make clean

make

./darknet detector demo cfg/face.data cfg/yolo-face.cfg yolo-face_final.weight //具体指令以具体情况为准

要点6：一次只能出现一张人脸！！！

采集结束时，ctrl+z结束进程就行。

### 训练部分：
1.把所有JPEG放到JPEGimages里

2.把txt按照class分别放在某个文件夹下的各自的文件夹中（就是每个人的txt要分开放）

3.convert.py可将坐标信息转成训练信息。注意按顺序填写classes，修改34、35行的坐标txt的路径（改一次跑一次）和输出路径（可以统一为labels）。cls填写对应class的名称。保存运行。

4.每个class都要重新修改cls运行convert.py（也就是说convert.py中的classes是为了确定class的index，真正生成的class是cls中填写的。有几个人就得改几次cls和34行的路径，跑几遍，生成几个list）

由于会直接生成记录文件路径的list，这一步最好放在服务器上运行。

*_list > train.txt

训练时，调整好参数之后，直接将train指过去就好了，估计也不用加什么vaild了。

Nohup ./darknet detector train cfg/face.data cfg/yolov3-face.cfg darknet53.conv.74 &


## -----------------新方案-----------------
## -----------------以下为旧方案-----------------
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
将文件夹整理好之后上传，之后的步骤在服务器上操作
<br>

### 2.使用voc_txt.c:
一个得到上述txt文件的一个demo。注意使用时修改相关内容。生成的内容会带有路径信息和后缀，需要手动去除。（文本替换）其中，*[^x]代表除了x之外的所有。可根据需要改动各txt中的内容。
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
backup = backup #务必确保权重文件输出地址存在。默认在darknet根目录下的backup
#### data/voc.names:
修改标签名
#### cfg/yolov3-voc.cfg：
修改三处[yolo]前后文中的classes和filters=（3*（classes+5））。注意最前面的Testing部分要注释掉，Training部分的解注。bath和subdivisions根据显存大小进行修改（越大越快，也越容易爆显存。TITAN XP上，选择了32，16）
<br>
<br>

### 5.开始训练：
nohup ./darknet detector train cfg/voc.data cfg/yolov3-voc.cfg darknet53.conv.74 -gpus 0,1 &<br>
该指令将会将训练任务放在后台。打开darknet文件夹下的nohup.out可以查看日志。
<br>
<br>
## -----------------旧方案-----------------
### 特别注意：
在源码src/data.c中，fill_truth_detection函数是用于利用图片路径获取labels路径。它会将“JPEGImages”换成“labels”，“jpg”换成“txt”，“JPEG”换成“txt”。如果原本的文件名后缀为小写的“jpeg”，注意修改源码
<br>

## 训练中
### 每秒刷新显卡监视窗口：
watch -n 1 nvidia-smi
### 刷新查看日志文件nohup.out:
tail -f nohup.out

