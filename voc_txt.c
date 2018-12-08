#include <cstdlib>
int main()
{   
   system("ls ./VOCdevkit/VOC2007/JPEGImages/*[^1].jpg > ./trainval.txt");
   system("ls ./VOCdevkit/VOC2007/JPEGImages/*0.jpg ./VOCdevkit/VOC2007/JPEGImages/*5.jpg > ./val.txt");
   system("ls ./VOCdevkit/VOC2007/JPEGImages/*[^0^1^5].jpg > ./train.txt");
   system("ls ./VOCdevkit/VOC2007/JPEGImages/*1.jpg > ./test.txt");
   return 0;
}
//ls 18*8.jpg 0*.jpg >test.txt
//ls 18*[^058].jpg > train.txt
//ls 18*0.jpg 18*5.jpg > val.txt

