#include <cstdlib>
int main()
{   
   system("ls ./VOCdevkit/VOC2007/JPEGImages *.jpg > ./trainval.txt");
   system("ls ./VOCdevkit/VOC2007/JPEGImages *0.jpg *5.jpg > ./val.txt");
   system("ls ./VOCdevkit/VOC2007/JPEGImages *[^05].jpg > ./train.txt");
   return 0;
}
//ls 18*8.jpg 0*.jpg >test.txt
//ls 18*[^058].jpg > train.txt
//ls 18*0.jpg 18*5.jpg > val.txt

