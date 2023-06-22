#include<bits/stdc++.h>
using namespace std;

string command;//用户指令

vector<string> path_list;

struct File{//定义文件信息
    string filename;
    int size;
    string content="";
    time_t time;
};

struct User{//定义用户信息
    string user_name;
    string pwd; 
};


struct Dir{ //目录信息
    string dir_name;
    vector<struct File> file;
    vector<struct Dir> dir;
};

struct Dir_Node{//目录联系信息
    struct Dir dir;
    vector<struct Dir_Node*> child;
    struct Dir_Node *parent;
};

struct Thread_Open_File_Table{//进程打开文件列表
    struct File file;
    int permission; //1读，2写，3读写 
    char* p;//读写指针
    int index;//系统索引号
};

struct Sys_Open_File_Table{//系统打开文件列表
    struct File file;
    int permission; //1读，2写，3读写 
    char* p;//读写指针
    int index;//系统索引号
    int count;//引用计数
};


vector<struct User> MFD; //主目录





void Welcome();
void Command_Judge(string command); //判断指令
void Create_User();//创建用户
void Show_User(); //显示用户信息
void Login_In(struct User u); //用户登录判定
void Dir(); ////显示当前目录下的文件信息
void Insert_Node_Dir();
void Cd(string dir_name);
void Cd_Rollback();
void MkDir(string dir_name);
void Os_Open(string filename);
void Os_Read();
void Os_Write();
void Os_Create(string fileName);
void Os_Close();
void showU();
void ShowFileInfo();

struct Dir_Node *pDir_root=new Dir_Node;
struct Dir_Node *pCurDir=pDir_root; //当前目录
vector<struct Sys_Open_File_Table> SOFT; //系统打开文件表
vector<struct Thread_Open_File_Table> TOFT;//进程打开文件表
time_t now_time; //时间戳变量
string Login_In_status="";//登录状态

int main(){
    Welcome();
    pDir_root->dir.dir_name="root";
    pDir_root->parent=nullptr;
    path_list.push_back("root");
    while(1){
        now_time=time(NULL);//实时时间戳

        for(int i=0;i<path_list.size();i++){
            if(i==path_list.size()-1)
                cout<<path_list[i];
            else
                cout<<path_list[i]<<"/";
        }
        if(Login_In_status!="")
            cout<<"("<<Login_In_status<<")";
        cout<<": >>> ";

        cin>>command;
        Command_Judge(command);
    }
}

void Command_Judge(string command){
    if(command=="help"){
        cout<<"dir-----显示当前目录文件"<<endl;
        cout<<"mkdir-----新建目录"<<endl;
        cout<<"cd-----切换到指定目录"<<endl;
        cout<<"cd..-----回退一级目录"<<endl;
        cout<<"del-----删除文件"<<endl;
        cout<<"open-----打开文件"<<endl;
        cout<<"create-----新建文件"<<endl;
        cout<<"read-----读文件"<<endl;
        cout<<"write-----写文件"<<endl;        
        cout<<"close-----关闭文件"<<endl;
        cout<<"create_user-----创建用户"<<endl;
        cout<<"login-----用户登录"<<endl;
        cout<<"showU-----显示用户信息"<<endl;
        cout<<"showF-----显示文件详细信息"<<endl;
        cout<<endl;
    }
    else if(command=="dir"){
        Dir();
        cout<<endl;
    }
    else if(command=="exit"){
        system("pause");
    }
    else if(command=="create_user"){

        Create_User();

        cout<<endl;
    }
    else if(command=="login"){
        struct User u;
        cout<<"输入用户名"<<endl;
        cin>>u.user_name;
        cout<<"输入密码"<<endl;
        cin>>u.pwd;
        Login_In(u);
    }
    else if(command=="cd"){
        string dir_name;
        cin>>dir_name;
        Cd(dir_name);

    }
    else if(command=="cd.."){
        Cd_Rollback();
    }
    else if(command=="mkdir"){
        cout<<"输入新建目录名"<<endl;
        string dir_name;
        cin>>dir_name;
        MkDir(dir_name);
        
    }
    else if(command=="create"){
        string fileName;
        cout<<"输入文件名"<<endl;
        cin>>fileName;
        while(fileName.find("/")!=std::string::npos){
            cout<<"文件名不能包含'/' !!!"<<endl;
            cout<<"输入文件名"<<endl;
            cin>>fileName;
        }
        while(fileName.find(".")==std::string::npos||fileName.find(".")==fileName.size()-1){
            cout<<"文件必须有后缀 !!!"<<endl;
            cout<<"输入文件名"<<endl;
            cin>>fileName;
        }         
       
        Os_Create(fileName);
    }
    else if(command=="open"){

        cout<<"输入文件名"<<endl;
        string fileName;
        cin>>fileName;
        Os_Open(fileName);
    }
    else if(command=="write"){
        Os_Write();
    }
    else if(command=="read"){
        Os_Read();
    }
    else if(command=="showU"){
        showU();
    }
    else if(command=="showF"){
        ShowFileInfo();
    }
    else{
        cout<<"不是命令"<<endl;
        cout<<endl;
    }
    
}

    


void Login_In(struct User u){
    bool flag=false;
    for(int i=0;i<MFD.size();i++){
        if(MFD[i].user_name==u.user_name){
            if(MFD[i].pwd==u.pwd){
                cout<<"登录成功"<<endl;
                Login_In_status=u.user_name;//登录状态改变
                cout<<endl;
                flag=true;
                for(int i=0;i<pCurDir->child.size();i++){
                    if(pCurDir->child[i]->dir.dir_name==u.user_name){
                        pCurDir=pCurDir->child[i];
                        path_list.push_back(u.user_name);
                        break;
                    }
                }
                break;
            }
            else{
                cout<<"密码错误"<<endl;
                cout<<endl;
                flag=true;
                break;
            }

        }
    }
    if(flag==false)
        cout<<"用户名不存在"<<endl;
        cout<<endl;
}


void Dir(){
    for(int i=0;i<pCurDir->child.size();i++){
        cout<<pCurDir->child[i]->dir.dir_name<<endl;
    }
    for(int i=0;i<pCurDir->dir.file.size();i++){
        cout<<pCurDir->dir.file[i].filename<<endl;
    }
}

void Insert_Node_Dir(struct Dir_Node *pcurDir, struct Dir newDir){

    struct Dir_Node *child_node=new Dir_Node;
    child_node->dir=newDir;
    child_node->parent=pcurDir;

    pcurDir->child.push_back(child_node);
}
void Insert_Node_File(struct Dir_Node *pcurDir, struct Dir newDir){

    struct Dir_Node *child_node=new Dir_Node;
    child_node->dir.file=newDir.file;
    child_node->parent=pcurDir;

    pcurDir->child.push_back(child_node);
}

void Create_User(){
    if(pCurDir->parent!=nullptr){
        cout<<"非根目录无法创建用户!!!"<<endl;
        return;
    }
    struct User u;
    cout<<"输入用户名"<<endl;
    cin>>u.user_name;
    cout<<"输入密码"<<endl;
    cin>>u.pwd;   
    for(int i=0;i<MFD.size();i++){
        if(MFD[i].user_name==u.user_name){
            cout<<"用户名已存在"<<endl;
            cout<<endl;
            return;
        }
    }
    MFD.push_back(u);

    struct Dir_Node *dn=new Dir_Node;
    struct Dir d;
    d.dir_name=u.user_name;
    dn->dir=d;
    Insert_Node_Dir(pCurDir, dn->dir);
    cout<<"创建成功!"<<endl;
}

void Cd(string dir_name){
    if(pCurDir->parent==nullptr){//判断是否为根目录
        if(Login_In_status=="no"){//判断是否登录
            cout<<"请先登录"<<endl;
            cout<<endl;
                return;
        }
        else{
            if(Login_In_status==dir_name){//判断是否为当前用户
                for(int i=0;i<pCurDir->child.size();i++){
                    if(pCurDir->child[i]->dir.dir_name==dir_name){
                        pCurDir=pCurDir->child[i];
                        path_list.push_back(dir_name);
                        return;
                    }  
                }
            }
        }
            
    }
    
    bool flag=false;
    for(int i=0;i<pCurDir->child.size();i++){
        if(pCurDir->child[i]->dir.dir_name==dir_name){
            pCurDir=pCurDir->child[i];
            path_list.push_back(dir_name);
            flag=true;
            break;
        }  
    }
    if(flag==false)
        cout<<"目录不存在"<<endl;
        cout<<endl;
}

void Cd_Rollback(){
    if(pCurDir->parent==nullptr)
        cout<<"已经到达最顶级目录！"<<endl;
    else{
        pCurDir=pCurDir->parent;
        path_list.pop_back();
    }
}

void MkDir(string dir_name){
    if(pCurDir->parent==nullptr){
        cout<<"根目录无法创建非用户目录!!!"<<endl;
        return;
    }    
    for(int i=0;i<pCurDir->child.size();i++){
        if(pCurDir->child[i]->dir.dir_name==dir_name){
            cout<<"目录已存在"<<endl;
            cout<<endl;
            return;
        }
    }
    struct Dir_Node *dn=new Dir_Node;
    struct Dir d;
    d.dir_name=dir_name;
    dn->dir=d;
    Insert_Node_Dir(pCurDir, dn->dir);
    cout<<"创建成功"<<endl;
}


void Os_Open(string filename){
    int f=0;
    for(int i=0;i<pCurDir->dir.file.size();i++){
        if(pCurDir->dir.file[i].filename==filename){
            f=1;
            break;
        }
    }
    if(f==0){
        cout<<"文件不存在"<<endl;
        return;
    }
    
    struct Thread_Open_File_Table toft;
    struct Sys_Open_File_Table soft;
    bool flag=false;//标志系统打开文件表是否存在该文件
    cout<<"权限: 1:读   2.写    3.读写"<<endl;
        int s;
        cin>>s;//声明权限
        if(s==1)
            toft.permission=1;
        else if(s==2)
            toft.permission=2;
        else if(s==3)
            toft.permission=3;
        else{
            cout<<"输入错误"<<endl;
            cout<<endl;
            return;
        }
        //在系统打开表中寻找,根据文件名
        for(int i=0;i<SOFT.size();i++){
            if(SOFT[i].file.filename==filename){//存在则计数器加一，并复制到进程的打开文件表中
                soft.file.filename=filename;
                SOFT[i].count++;
                toft.file.filename=filename;
                toft.index=i; //系统打开文件表的编号作为进程打开文件表的索引
                TOFT.push_back(toft);
                flag=true;
                break;
            }
        }
        if(flag==false){//不存在则，加入到系统的打开文件表，并复制到进程的打开文件表中
            soft.file.filename=filename;
            soft.count=1;
            SOFT.push_back(soft);
            toft.file.filename=filename;
            toft.index=SOFT.size()-1;
            TOFT.push_back(toft);
        }
        cout<<"打开成功"<<endl;
}

void Os_Write(){
    string fileName;
    int flag=false;
    int flag2=false;
    cout<<"输入文件名"<<endl;
    cin>>fileName;
    for(int i=0;i<pCurDir->dir.file.size();i++){
        if(pCurDir->dir.file[i].filename==fileName){//寻找目录是否存在该文件名
            flag=true; //存在
            // 查找进程打开文件表是否存在
            for(int j=0;j<TOFT.size();j++){
                if(TOFT[j].file.filename==fileName){
                    flag2=true; //存在
                    if(TOFT[j].permission==2||TOFT[j].permission==3){//判断权限
                        cout<<"输入内容"<<endl;
                        string content;
                        cin>>content;
                        pCurDir->dir.file[i].content=content;
                        pCurDir->dir.file[i].size=content.size();
                        break;
                    }
                    else{
                        cout<<"没有写权限"<<endl;
                        return;
                    }
                }
            }
            if(flag2==false){
                cout<<"文件未打开"<<endl;
                return;
            }
        }
    }
    if(flag==false){
        cout<<"当前目录不存在该文件";
        return;
    }
    cout<<"写入成功"<<endl;
}

void Os_Read(){
    bool flag=false;
    bool flag2=false;
    string fileName;
    cout<<"输入文件名"<<endl;
    cin>>fileName;
    for(int i=0;i<pCurDir->dir.file.size();i++){
        if(pCurDir->dir.file[i].filename==fileName){//判断当前目录是否存在该文件
            flag=true;
            for(int j=0;j<TOFT.size();j++){
                if(TOFT[j].file.filename==fileName){//判断进程打开文件表是否存在该文件
                    flag2=true;
                    if(TOFT[j].permission==1||TOFT[j].permission==3){//判断权限
                        cout<<pCurDir->dir.file[i].content<<endl;
                        break;
                    }
                    else{
                        cout<<"没有读权限"<<endl;
                        return;
                    }
                }
            }
            if(flag2==false){
                cout<<"文件未打开"<<endl;
                return;
            }
        }
    }
    
    if(flag==false){
        cout<<"当前目录不存在该文件"<<endl;
        return;
    }
}

void Os_Del(string filename){
    int flag=false;
    for(int i=0;i<pCurDir->dir.file.size();i++){
        if(pCurDir->dir.file[i].filename==filename){
            flag=true;
            pCurDir->dir.file.erase(pCurDir->dir.file.begin()+i);
            break;
        }
    }
    if(flag==false){
        cout<<"当前目录不存在该文件"<<endl;
        return;
    }
    cout<<"删除成功"<<endl;
}

void Os_Create(string fileName){
    for(int i=0;i<pCurDir->dir.file.size();i++){
        if(pCurDir->dir.file[i].filename==fileName){
            cout<<"当前目录已存在该文件，无法继续创建"<<endl;
            return;
        }
    }        
    struct File file;
    file.filename=fileName;
    file.size=file.content.size();
    file.time=now_time;
    pCurDir->dir.file.push_back(file);
    cout<<"创建成功"<<endl;
}

void Show_User(){
    for(int i=0;i<MFD.size();i++){
        cout<<MFD[i].user_name<<endl;
    }
}

void showU(){

    cout<<"-------------------------------------------------------------------"<<endl;
    cout<<"用户名"<<"\t"<<endl;
    for(int i=0;i<MFD.size();i++){
        cout<<MFD[i].user_name<<endl;
    }
    cout<<"-------------------------------------------------------------------"<<endl;

}

void Os_Close(){
    string filename;
    cout<<"输入文件名"<<endl;
    for(int i=0;i<TOFT.size();i++){
        if(TOFT[i].file.filename==filename){
            SOFT[TOFT[i].index].count--;
            if(SOFT[TOFT[i].index].count==0)
                SOFT.erase(SOFT.begin()+TOFT[i].index);
            TOFT.erase(TOFT.begin()+i);
            cout<<"关闭成功"<<endl;
            return;
        }
    }
}

void ShowFileInfo(){
    cout<<"-------------------------------------------------------------------"<<endl;
    cout.setf(ios::right);
    cout<<"文件名\t\t文件大小\t创建时间戳\n";
    for(int i=0;i<pCurDir->dir.file.size();i++){
        cout<<pCurDir->dir.file[i].filename<<"\t\t"<<pCurDir->dir.file[i].size<<"\t\t"<<pCurDir->dir.file[i].time<<endl;
    }
        cout.unsetf(ios::right);
    cout<<"-------------------------------------------------------------------"<<endl;
}

void Welcome(){
    cout<<"-------------------------------------------------------------------"<<endl;
    cout<<"欢迎使用文件系统"<<endl;
    cout<<"输入help以查看帮助"<<endl;
    cout<<"-------------------------------------------------------------------"<<endl;
}
