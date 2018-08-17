#include "network.h"
#include "detection_layer.h"
#include "region_layer.h"
#include "cost_layer.h"
#include "utils.h"
#include "parser.h"
#include "box.h"
#include "image.h"
#include "yujianke.h"
#include <sys/time.h>
#include "mysystem.h"
#include <math.h>
#include <string.h>
#include <time.h>

#define YUJIANKE 1

#ifdef OPENCV

static char **yujianke_names;
static image **yujianke_alphabet;
static int yujianke_classes;

static network *net;
static image buff [3];
static image buff_letter[3];
static int buff_index = 0;
static CvCapture * cap;
static IplImage  * ipl;
static float fps = 0;
static float yujianke_thresh = 0;
static float yujianke_hier = .5;
static int running = 0;

static int yujianke_frame = 3;
static int yujianke_index = 0;
static float **predictions;
static float *avg;
static int yujianke_done = 0;
static int yujianke_total = 0;
static int total_person_num=0;
static int flag=0;
static int temp=0;
double yujianke_time;
static char continue_flag[255]={"continue"}
;
FILE *fp = NULL;
time_t tim;
struct tm *at;
char now[80];
char start_time[80];
detection *get_network_boxes(network *net, int w, int h, float thresh, float hier, int *map, int relative, int *num);

int yu_size_network(network *net)
{
    int i;
    int count = 0;
    for(i = 0; i < net->n; ++i){
        layer l = net->layers[i];
        if(l.type == YOLO || l.type == REGION || l.type == DETECTION){
            count += l.outputs;
        }
    }
    return count;
}

void yu_remember_network(network *net)
{
    int i;
    int count = 0;
    for(i = 0; i < net->n; ++i){
        layer l = net->layers[i];
        if(l.type == YOLO || l.type == REGION || l.type == DETECTION){
            memcpy(predictions[yujianke_index] + count, net->layers[i].output, sizeof(float) * l.outputs);
            count += l.outputs;
        }
    }
}

detection *yu_avg_predictions(network *net, int *nboxes)
{
    int i, j;
    int count = 0;
    fill_cpu(yujianke_total, 0, avg, 1);
    for(j = 0; j < yujianke_frame; ++j){
        axpy_cpu(yujianke_total, 1./yujianke_frame, predictions[j], 1, avg, 1);
    }
    for(i = 0; i < net->n; ++i){
        layer l = net->layers[i];
        if(l.type == YOLO || l.type == REGION || l.type == DETECTION){
            memcpy(l.output, avg + count, sizeof(float) * l.outputs);
            count += l.outputs;
        }
    }
    detection *dets = get_network_boxes(net, buff[0].w, buff[0].h, yujianke_thresh, yujianke_hier, 0, 1, nboxes);
    return dets;
}

void *yu_detect_in_thread(void *ptr)
{
    int person_num=0;
    running = 1;
    float nms = .4;
    
    layer l = net->layers[net->n-1];
    float *X = buff_letter[(buff_index+2)%3].data;
    network_predict(net, X);

    /*
       if(l.type == DETECTION){
       get_detection_boxes(l, 1, 1, yujianke_thresh, probs, boxes, 0);
       } else */
    yu_remember_network(net);
    detection *dets = 0;
    int nboxes = 0;
    dets = yu_avg_predictions(net, &nboxes);


    /*
       int i,j;
       box zero = {0};
       int classes = l.classes;
       for(i = 0; i < yujianke_detections; ++i){
       avg[i].objectness = 0;
       avg[i].bbox = zero;
       memset(avg[i].prob, 0, classes*sizeof(float));
       for(j = 0; j < yujianke_frame; ++j){
       axpy_cpu(classes, 1./yujianke_frame, dets[j][i].prob, 1, avg[i].prob, 1);
       avg[i].objectness += dets[j][i].objectness * 1./yujianke_frame;
       avg[i].bbox.x += dets[j][i].bbox.x * 1./yujianke_frame;
       avg[i].bbox.y += dets[j][i].bbox.y * 1./yujianke_frame;
       avg[i].bbox.w += dets[j][i].bbox.w * 1./yujianke_frame;
       avg[i].bbox.h += dets[j][i].bbox.h * 1./yujianke_frame;
       }
    //copy_cpu(classes, dets[0][i].prob, 1, avg[i].prob, 1);
    //avg[i].objectness = dets[0][i].objectness;
    }
     */

    if (nms > 0) do_nms_obj(dets, nboxes, l.classes, nms);

    printf("\033[2J");
    printf("\033[1;1H");
    printf("\nFPS:%.1f\n",fps);
    printf("Objects:\n\n");
    image display = buff[(buff_index+2) % 3];
    person_num=draw_detections(display, dets, nboxes, yujianke_thresh, yujianke_names, yujianke_alphabet, yujianke_classes);
    free_detections(dets, nboxes);
    if(temp>person_num){
	if(flag!=temp-person_num){
            total_person_num=total_person_num+temp-person_num;
        }
    }
    printf("当前检测到的人数：%d\n检测到的已通过指定地点的总人数：%d\n", person_num,total_person_num);
    flag=abs(temp-person_num);
    temp=person_num;
    /*if(person_num>0){
	mysystem();	
    }*/
    yujianke_index = (yujianke_index + 1)%yujianke_frame;
    running = 0;
    return 0;
}

void *yu_fetch_in_thread(void *ptr)
{
    int status = fill_image_from_stream(cap, buff[buff_index]);
    letterbox_image_into(buff[buff_index], net->w, net->h, buff_letter[buff_index]);
    if(status == 0) yujianke_done = 1;
    return 0;
}

void *yu_display_in_thread(void *ptr)
{
    show_image_cv(buff[(buff_index + 1)%3], "人数检测系统", ipl);
    int c = cvWaitKey(1);
    if (c != -1) c = c%256;
    if (c == 27) {
        yujianke_done = 1;
        return 0;
    } else if (c == 82) {
        yujianke_thresh += .02;
    } else if (c == 84) {
        yujianke_thresh -= .02;
        if(yujianke_thresh <= .02) yujianke_thresh = .02;
    } else if (c == 83) {
        yujianke_hier += .02;
    } else if (c == 81) {
        yujianke_hier -= .02;
        if(yujianke_hier <= .0) yujianke_hier = .0;
    }
    return 0;
}

void *yu_display_loop(void *ptr)
{
    
    while(1){
        yu_display_in_thread(0);
    }
}

void *yu_detect_loop(void *ptr)
{
    while(strcmp(continue_flag,"close")){
	fp = fopen("./flag", "r");
	fgets(buff, 255, (FILE*)fp);
	fclose(fp);
        yu_detect_in_thread(0);
    }
}

void yujianke(char *cfgfile, char *weightfile, float thresh, int cam_index, const char *filename, char **names, int classes, int delay, char *prefix, int avg_frames, float hier, int w, int h, int frames, int fullscreen)
{
    //yujianke_frame = avg_frames;
    time(&tim);
    at=localtime(&tim);
    strftime(start_time,79,"%Y-%m-%d_%H:%M:%S\n",at);
    image **alphabet = load_alphabet();
    yujianke_names = names;
    yujianke_alphabet = alphabet;
    yujianke_classes = classes;
    yujianke_thresh = thresh;
    yujianke_hier = hier;
    printf("人数检测系统\n");
    net = load_network(cfgfile, weightfile, 0);
    set_batch_network(net, 1);
    pthread_t detect_thread;
    pthread_t fetch_thread;

    srand(2222222);

    int i;
    yujianke_total = yu_size_network(net);
    predictions = calloc(yujianke_frame, sizeof(float*));
    for (i = 0; i < yujianke_frame; ++i){
        predictions[i] = calloc(yujianke_total, sizeof(float));
    }
    avg = calloc(yujianke_total, sizeof(float));

    if(filename){
        printf("video file: %s\n", filename);
        cap = cvCaptureFromFile(filename);
    }else{
        cap = cvCaptureFromCAM(cam_index);

        if(w){
            cvSetCaptureProperty(cap, CV_CAP_PROP_FRAME_WIDTH, w);
        }
        if(h){
            cvSetCaptureProperty(cap, CV_CAP_PROP_FRAME_HEIGHT, h);
        }
        if(frames){
            cvSetCaptureProperty(cap, CV_CAP_PROP_FPS, frames);
        }
    }

    if(!cap) error("Couldn't connect to webcam.\n");

    buff[0] = get_image_from_stream(cap);
    buff[1] = copy_image(buff[0]);
    buff[2] = copy_image(buff[0]);
    buff_letter[0] = letterbox_image(buff[0], net->w, net->h);
    buff_letter[1] = letterbox_image(buff[0], net->w, net->h);
    buff_letter[2] = letterbox_image(buff[0], net->w, net->h);
    ipl = cvCreateImage(cvSize(buff[0].w,buff[0].h), IPL_DEPTH_8U, buff[0].c);

    int count = 0;
    if(!prefix){
        cvNamedWindow("人数检测系统", CV_WINDOW_NORMAL); 
        if(fullscreen){
            cvSetWindowProperty("人数检测系统", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
        } else {
            cvMoveWindow("人数检测系统", 0, 0);
            cvResizeWindow("人数检测系统", 640, 480);
        }
    }

    yujianke_time = what_time_is_it_now();
    system("gnome-terminal --geometry 80x24--10+14 -e ./stop");
    while(!yujianke_done&&strcmp(continue_flag,"close")!=0&&strcmp(continue_flag,"close_without_saving")!=0){
	if(strcmp(continue_flag,"restart")==0){
		total_person_num=0;
		strcpy(continue_flag,"continue");	
	}
	if(strcmp(continue_flag,"pause")==0){
		printf("系统已暂停\n");
		while(strcmp(continue_flag,"continue")!=0){
			fp = fopen("./flag", "r");
			fgets(continue_flag, 255, (FILE*)fp);
			fclose(fp);
			sleep(1);
		}
		system("clear");
	}
	if(strcmp(continue_flag,"save")==0){
		time(&tim);
		at=localtime(&tim);
		strftime(now,79,"%Y-%m-%d_%H:%M:%S",at);
		fp = fopen("./人数统计结果.txt", "a+");
		fprintf(fp,"系统运行期间%s--%s检测到经过规定区域的人数：%d\n\n",start_time,now,total_person_num);
		fclose(fp);
		strcpy(continue_flag,"continue");
		fp = fopen("./flag", "w+");
		fprintf(fp, "%s",continue_flag);
		fclose(fp);
		continue;
	}
	fp = fopen("./flag", "r");
	fgets(continue_flag, 255, (FILE*)fp);
	fclose(fp);
        buff_index = (buff_index + 1) %3;
        if(pthread_create(&fetch_thread, 0, yu_fetch_in_thread, 0)) error("Thread creation failed");
        if(pthread_create(&detect_thread, 0, yu_detect_in_thread, 0)) error("Thread creation failed");
        if(!prefix){
            fps = 1./(what_time_is_it_now() - yujianke_time);
            yujianke_time = what_time_is_it_now();
            yu_display_in_thread(0);
        }else{
            char name[256];
            sprintf(name, "%s_%08d", prefix, count);
            save_image(buff[(buff_index + 1)%3], name);
        }
        pthread_join(fetch_thread, 0);
        pthread_join(detect_thread, 0);
        ++count;
    }
	if(strcmp(continue_flag,"close_without_saving")!=0){
	time(&tim);
	at=localtime(&tim);
	strftime(now,79,"%Y-%m-%d_%H:%M:%S",at);
	fp = fopen("./人数统计结果.txt", "a+");
	fprintf(fp,"系统运行期间%s--%s检测到经过规定区域的人数：%d\n\n",start_time,now,total_person_num);
	fclose(fp);
	printf("\n人数统计结果已保存至系统根目录中,结果仅供参考\n");
	}
	
    printf("\n人数监测系统已关闭\n");
}

/*
   void yujianke_compare(char *cfg1, char *weight1, char *cfg2, char *weight2, float thresh, int cam_index, const char *filename, char **names, int classes, int delay, char *prefix, int avg_frames, float hier, int w, int h, int frames, int fullscreen)
   {
   yujianke_frame = avg_frames;
   predictions = calloc(yujianke_frame, sizeof(float*));
   image **alphabet = load_alphabet();
   yujianke_names = names;
   yujianke_alphabet = alphabet;
   yujianke_classes = classes;
   yujianke_thresh = thresh;
   yujianke_hier = hier;
   printf("Demo\n");
   net = load_network(cfg1, weight1, 0);
   set_batch_network(net, 1);
   pthread_t detect_thread;
   pthread_t fetch_thread;

   srand(2222222);

   if(filename){
   printf("video file: %s\n", filename);
   cap = cvCaptureFromFile(filename);
   }else{
   cap = cvCaptureFromCAM(cam_index);

   if(w){
   cvSetCaptureProperty(cap, CV_CAP_PROP_FRAME_WIDTH, w);
   }
   if(h){
   cvSetCaptureProperty(cap, CV_CAP_PROP_FRAME_HEIGHT, h);
   }
   if(frames){
   cvSetCaptureProperty(cap, CV_CAP_PROP_FPS, frames);
   }
   }

   if(!cap) error("Couldn't connect to webcam.\n");

   layer l = net->layers[net->n-1];
   yujianke_detections = l.n*l.w*l.h;
   int j;

   avg = (float *) calloc(l.outputs, sizeof(float));
   for(j = 0; j < yujianke_frame; ++j) predictions[j] = (float *) calloc(l.outputs, sizeof(float));

   boxes = (box *)calloc(l.w*l.h*l.n, sizeof(box));
   probs = (float **)calloc(l.w*l.h*l.n, sizeof(float *));
   for(j = 0; j < l.w*l.h*l.n; ++j) probs[j] = (float *)calloc(l.classes+1, sizeof(float));

   buff[0] = get_image_from_stream(cap);
   buff[1] = copy_image(buff[0]);
   buff[2] = copy_image(buff[0]);
   buff_letter[0] = letterbox_image(buff[0], net->w, net->h);
   buff_letter[1] = letterbox_image(buff[0], net->w, net->h);
   buff_letter[2] = letterbox_image(buff[0], net->w, net->h);
   ipl = cvCreateImage(cvSize(buff[0].w,buff[0].h), IPL_DEPTH_8U, buff[0].c);

   int count = 0;
   if(!prefix){
   cvNamedWindow("Demo", CV_WINDOW_NORMAL); 
   if(fullscreen){
   cvSetWindowProperty("Demo", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
   } else {
   cvMoveWindow("Demo", 0, 0);
   cvResizeWindow("Demo", 1352, 1013);
   }
   }

   yujianke_time = what_time_is_it_now();

   while(!yujianke_done){
buff_index = (buff_index + 1) %3;
if(pthread_create(&fetch_thread, 0, yu_fetch_in_thread, 0)) error("Thread creation failed");
if(pthread_create(&detect_thread, 0, yu_detect_in_thread, 0)) error("Thread creation failed");
if(!prefix){
    fps = 1./(what_time_is_it_now() - yujianke_time);
    yujianke_time = what_time_is_it_now();
    yu_display_in_thread(0);
}else{
    char name[256];
    sprintf(name, "%s_%08d", prefix, count);
    save_image(buff[(buff_index + 1)%3], name);
}
pthread_join(fetch_thread, 0);
pthread_join(detect_thread, 0);
++count;
}
}
*/
#else
void yujianke(char *cfgfile, char *weightfile, float thresh, int cam_index, const char *filename, char **names, int classes, int delay, char *prefix, int avg, float hier, int w, int h, int frames, int fullscreen)
{
    fprintf(stderr, "Demo needs OpenCV for webcam images.\n");
}
#endif

