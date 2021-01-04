typedef int (*fn_ptr)();//函数指针 定义方法：返回值类型(*指针变量名)(形参列表)

extern int sys_fork();
extern int sys_write();
//int (*sys_call_table[1])() = {sys_fork}; 
fn_ptr sys_call_table[]={sys_fork,sys_write};
