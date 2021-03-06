#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

using std::cout;
using std::cin;
using std::endl;

//get file name not the path
char *getFilename(const char *filepath){
	int len = strlen(filepath);
	char *temp = new char[len];
	for(size_t i=len-1;; i--){
		if(filepath[i] != '/'){
			strcat(temp,&filepath[i]);
		}else{
			break;
		}
	}
	return temp;
}


int main(int argc, char* argv[]){

	if(argc != 3){
		cout<<"usage:tcp file1 <file2|dir>"<<endl;
		return 1;
	}

	struct stat buf1;
	struct stat buf2;
	const char *filepath_1 = argv[1];
	const char *filepath_2 = argv[2];
	int filedes_1,filedes_2;
	int buf_size = 1024;
	char buf[buf_size];

	if (stat(filepath_1,&buf1) < 0){
		cout<<"File1 not exists!"<<endl;
		return 1;
	}

	if(S_ISDIR(buf1.st_mode)){
		cout<<"Cannot copy a dir"<<endl;
		return 1;
	}

	filedes_1 = open(filepath_1,O_RDONLY);

	if(S_ISREG(buf1.st_mode)){
		char temp = filepath_2[strlen(filepath_2)-1];
		if(temp == '/'){
			cout<<"file2 dir not exists"<<endl;

			//get file2 path
			char *name = getFilename(filepath_1);
			char *file2_name = new char[strlen(filepath_2)+strlen(name)+2];
			strcat(file2_name,filepath_2);
			strcat(file2_name,name);
			
			if(stat(file2_name,&buf2)<0){
				if(access(filepath_2,F_OK)<0){
					cout<<"Dir not exiets"<<endl;
					close(filedes_1);
					return 1;
				}else{
					if((filedes_2 = open(file2_name,O_WRONLY|O_CREAT|O_TRUNC,0644))<0){
						cout<<"Create File error!"<<endl;
						close(filedes_1);
						return 1;
					}
				}
			}else{
				cout<<"File path has existed,cover?"<<endl;
				char c;
				cin >> c;
				if(c != 'y' || c != 'Y'){
					close(filedes_1);
					return 1;
				}
				//get the file descriptor
				filedes_2 = open(file2_name,O_TRUNC);
			}
		}else{
			if(access(filepath_2,F_OK)<0){
				if((filedes_2=open(filepath_2,O_WRONLY|O_CREAT|O_TRUNC,0644))<0){
					cout<<"Create File Error!"<<endl;
					close(filedes_1);
					close(filedes_2);
					return 1;
				}
			}else{
				cout<<"File path has existed,cover?"<<endl;
				char c;
				cin >> c;
				if(c != 'y' || c != 'Y'){
					close(filedes_1);
					return 1;
				}
				//get the file descriptor
				filedes_2 = open(filepath_2,O_TRUNC);
			}
		}
	}

	int i;
	while(( i = read(filedes_1,buf,buf_size)) > 0){
		if((write(filedes_2, buf, i)) != i){
			cout << "Write to file2 error" << endl;
			close(filedes_1);
			close(filedes_2);
			return 1;
		}
	}

	close(filedes_1);
	close(filedes_2);
	return 0;
}