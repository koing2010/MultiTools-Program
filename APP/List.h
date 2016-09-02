/*
 * List.h
 *
 *  Created on: 2014-11-26
 *      Author: xiao70
 */

#ifndef LIST_H_
#define LIST_H_

typedef struct _LIST_ENTRY
{
	struct _LIST_ENTRY *Flink;
	struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY;

#include"includes.h"
void InitializeListHead(PLIST_ENTRY ListHead);

bool IsListEmpty(const LIST_ENTRY * ListHead);
bool RemoveEntryList(PLIST_ENTRY Entry);
PLIST_ENTRY RemoveHeadList(PLIST_ENTRY ListHead);
PLIST_ENTRY RemoveTailList(PLIST_ENTRY ListHead);
void InsertTailList(PLIST_ENTRY ListHead, PLIST_ENTRY Entry);
void InsertHeadList(PLIST_ENTRY ListHead, PLIST_ENTRY Entry);

#endif /* LIST_H_ */
