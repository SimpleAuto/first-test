// 链表插入节点
static inline void __list_add(struct list_head* new_entry,struct list_head* prev,struct list_head* next);
// 外部调用函数  实际调用__list_add函数
static inline void list_add(struct list_head* new_entry,struct list_head* head);
