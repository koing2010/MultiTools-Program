/*
 * List.c
 *
 *  Created on: 2014-11-26
 *      Author: xiao70
 */
#include"List.h"

void InitializeListHead(PLIST_ENTRY ListHead)	//ע:�������ʼ����������Ǳ�����ӵ�һ���ڵ�
{
	ListHead->Flink = ListHead->Blink = ListHead;
}

bool IsListEmpty(const LIST_ENTRY * ListHead)
{
	return (bool) (ListHead->Flink == ListHead); //��ΪTRUE��ʱ��Ϊ������
}

bool RemoveEntryList(PLIST_ENTRY Entry)
{
	PLIST_ENTRY Blink;
	PLIST_ENTRY Flink;

	Flink = Entry->Flink;
	Blink = Entry->Blink;
	Blink->Flink = Flink;
	Flink->Blink = Blink;
	return (bool) (Flink == Blink);
}

PLIST_ENTRY RemoveHeadList(PLIST_ENTRY ListHead) //ע:ɾ������ĵ�һ���ڵ㣬ע��������ڲ���head�Ǹ��ṹ�����
{
	PLIST_ENTRY Flink;
	PLIST_ENTRY Entry;

	Entry = ListHead->Flink;
	Flink = Entry->Flink;
	ListHead->Flink = Flink;
	Flink->Blink = ListHead;
	return Entry;
}

PLIST_ENTRY RemoveTailList(PLIST_ENTRY ListHead)
{
	PLIST_ENTRY Blink;
	PLIST_ENTRY Entry;

	Entry = ListHead->Blink;
	Blink = Entry->Blink;
	ListHead->Blink = Blink;
	Blink->Flink = ListHead;
	return Entry;
}

void InsertTailList(PLIST_ENTRY ListHead, PLIST_ENTRY Entry) //ע:������β����ӽڵ�
{
	PLIST_ENTRY Blink;

	Blink = ListHead->Blink;
	Entry->Flink = ListHead;
	Entry->Blink = Blink;
	Blink->Flink = Entry;
	ListHead->Blink = Entry;
}

void InsertHeadList(PLIST_ENTRY ListHead, PLIST_ENTRY Entry)
{
	PLIST_ENTRY Flink;

	Flink = ListHead->Flink;
	Entry->Flink = Flink;
	Entry->Blink = ListHead;
	Flink->Blink = Entry;
	ListHead->Flink = Entry;
}

