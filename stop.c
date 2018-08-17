#include<stdio.h>
#include<string.h>
#include<stdlib.h>
int main(){	
   FILE *fp = NULL;
   char flag[255];
   while(1){
	   printf("=============================================\n");
	   printf("             人像识别程序管理程序             \n");
	   printf("         输入“read”浏览目前的统计记录         \n");
	   printf("         输入“save”保存目前的统计记录         \n");
	   printf("         输入“clean”清除目前的统计记录        \n");
	   printf("         输入“close”关闭人像识别程序          \n");
	   printf(" 输入“close_without_saving”关闭人像识别程序   \n");
	   printf("         输入“pause”暂停人像识别程序          \n");
	   printf("        输入“continue”恢复人像识别程序        \n");
	   printf("           输入“restart”重置统计人数          \n");
           printf("               其他指令输入无效               \n");
	   printf("   若不慎关闭了此程序，可运行./stop重新启动    \n");
	   printf("=============================================\n");
	   printf("\n等待指令输入：");
	   scanf("%s",flag);
	   if(strcmp(flag,"read")==0){
		strcpy(flag,"continue");
		system("gnome-terminal --geometry 80x20+55--9 -x less ./人数统计结果.txt");
		system("clear");
		continue;
	   }
	   if(strcmp(flag,"clean")==0){
		printf("确定清空统计记录吗？N/Y:");
		scanf("%s",flag);
		if(strcmp(flag,"y")==0||strcmp(flag,"Y")==0||strcmp(flag,"YES")==0||strcmp(flag,"Yes")==0||strcmp(flag,"yes")==0){
			strcpy(flag,"");
			fp = fopen("./人数统计结果.txt", "w+");
		   	fprintf(fp, "%s",flag);
		   	fclose(fp);
			strcpy(flag,"continue");
			printf("已清除目前的统计记录,按回车继续管理人像识别系统");
			getchar();
			getchar();
			system("clear");
			strcpy(flag,"continue");
			continue;
			}
		else{
			printf("已取消清除目前的统计记录,按回车继续管理人像识别系统");
			getchar();
			getchar();
			system("clear");
			strcpy(flag,"continue");
			continue;
			}
	   }
	   fp = fopen("./flag", "w+");
	   fprintf(fp, "%s",flag);
	   fclose(fp);
	   if(strcmp(flag,"close")==0)
		return 0;
	   else if(strcmp(flag,"close_without_saving")==0)
		return 0;
	   else if(strcmp(flag,"pause")==0){
		printf("系统已暂停，按回车继续管理人像识别系统");
		getchar();
		getchar();
	   }
	   else if(strcmp(flag,"continue")==0){
		printf("系统已恢复，按回车继续管理人像识别系统");
		getchar();
		getchar();
	   }
	    else if(strcmp(flag,"save")==0){
		printf("人数统计结果已保存至系统根目录中,结果仅供参考，按回车继续");
		getchar();
		getchar();
	   }	
	   else if(strcmp(flag,"restart")==0){
		printf("人数统计结果已重置，按回车继续");
	   	getchar();
	   	getchar();
	   }
	   system("clear");
	   
}

}
