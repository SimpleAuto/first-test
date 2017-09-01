#ifndef _LIST_H_
#define _LIST_H_

// 链表头结点类型
struct list_head{
	struct list_head *next,*prev;
};

// typedef
typedef struct list_head list_head_t;

// 初始化头节点
#define INIT_LIST_HEAD(ptr) do { \
	(ptr)->next = (ptr); (ptr)->prev = (ptr); \
}while (0)

#define list_entry(ptr,type,member) \
	((type*)((char*)(ptr)-(unsigned long)(&((type*)0)->member)))

static inline void __list_add(struct list_head* new_entry,struct list_head* prev,struct list_head* next)
{
	next->prev = new_entry;
	new_entry->next = next;
	new_entry->prev = prev;
	prev->next = new_entry;
}

static inline void list_add(struct list_head* new_entry,struct list_head* head)
{
	__list_add(new_entry,head,head->next);
}

#endif // _LIST_H_
