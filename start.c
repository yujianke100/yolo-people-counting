#include <stdlib.h>
#include <stdio.h>
#include <string.h>
void main(){
	char input[10];
	int comp;
	printf("请选择USB摄像头或网络摄像头(默认USB)\n请输入usb或ip:\n");
	scanf("%s",input);
	comp=strcmp(input,"ip");
	if(comp==0)
	    system("./darknet detector yujianke cfg/coco.data cfg/yolov3.cfg yolov3.weights rtsp://admin:aa123456789@10.20.4.120/h264/ch1/sub/av_stream -w 640 -h 480");

	else
	    system("./darknet detector yujianke cfg/coco.data cfg/yolov3-tiny.cfg yolov3-tiny.weights -c 1 -w 640 -h 480");
	    
}
