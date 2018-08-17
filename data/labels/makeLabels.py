# -*- coding: utf-8 -*-    
import os   

#l=[]
#with open("coco.names") as list_in:
#    for line in list_in:
#        l.append(line)

l=["人", "飞机", "自行车", "鸟", "船", "瓶子", "大巴", "车", "猫", "椅子", "奶牛", "桌子", "狗", "房子", "摩托车", "盆栽", "绵羊", "沙发", "火车", "电视监视器"]


def make_labels(s): 
    i = 0 
    for word in l:   
        os.system("convert -fill black -background white -bordercolor white -border 4  -font /usr/share/fonts/truetype/arphic/ukai.ttc -pointsize %d label:\"%s\" \"byd_%d_%d.png\""%(s,word,i,s/12-1)) 
        i = i + 1 

for i in [12,24,36,48,60,72,84,96]:
    make_labels(i)
