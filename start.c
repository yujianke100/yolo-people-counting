#include<stdlib.h>
void main(){
	system("./darknet detector yujianke cfg/coco.data cfg/yolov3-tiny.cfg yolov3-tiny.weights -c 1 -w 640 -h 480");
}
