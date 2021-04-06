#include<cstdio>
#include<cstdlib>
#include<cstring>
using namespace std;

int main(int argc,char* argv[]){
   char* cmd = "./rmcv";
   int loop_cnt = -1;
   if(argc >= 2){
   	cmd = argv[1];
   }
   if(argc >= 3){
   	loop_cnt = atoi(argv[2]);
   }
   while(loop_cnt--){
   	system(cmd);
   }
   return 0;
}
