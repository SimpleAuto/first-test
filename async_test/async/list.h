#ifndef _LIST_H_
#define _LIST_H_

// 链表头结点类型
struct list_head
{
	struct list_head *next,*prev;
};

#define __builtin_prefetch(x,y,z) 1

// typedef
typedef struct list_head list_head_t;

// 初始化头节点
#define INIT_LIST_HEAD(ptr) do { \
	(ptr)->next = (ptr); (ptr)->prev = (ptr); \
}while (0)

/**
 * @brief get the struct for this entry
 * @param ptr the &struct list_head pointer.
 * @param type the type of the struct this is embedded in.
 * @param member the name of the list_struct within the struct.
 */
#define list_entry(ptr,type,member) \
	((type*)((char*)(ptr)-(unsigned long)(&((type*)0)->member)))

/**
 * @brief iterate over list of given type
 * @param pos the type * to use as a loop counter.
 * @param head the head for your list.
 * @param member the name of the list_struct within the struct.
 */
#define list_for_each_entry(pos,head,member)     \
    for (pos = list_entry((head)->next,typeof(*pos),member);  \
         &pos->member != (head);  \
         pos = list_entry(pos->member.next,typeof(*pos),member))

/*
 * Insert a new entry between two known consecutive entries. 
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_add(struct list_head* new_entry,struct list_head* prev,struct list_head* next)
{
	next->prev = new_entry;
	new_entry->next = next;
	new_entry->prev = prev;
	prev->next = new_entry;
}

/**
 * @brief add a new entry\n
 *        Insert a new entry after the specified head.
 *        This is good for implementing stacks.
 * @param new_entry new entry to be added
 * @param head list head to add it after
 */
static inline void list_add(struct list_head* new_entry,struct list_head* head)
{
	__list_add(new_entry,head,head->next);
}

#endif // _LIST_H_
