#pragma once

#define iceListRecord(address, type, field) \
   ((type*)((char*)(address) - (char*)(&((type*)0)->field)))

#define iceListForEach(position, head) \
   for (position = (head)->next; position != head; position = position->next)

#define iceListForEachSafe(position, n, head) \
   for (position = (head)->next, n = position->next; position != (head); position = n, n = position->next)

typedef struct iceListEntryT {
   struct iceListEntryT* prev;
   struct iceListEntryT* next;
} iceListEntryT;

typedef struct iceListEntryT iceListHeadT;

static void
iceListInit(iceListHeadT* self) {
   self->prev = (iceListEntryT*)self;
   self->next = (iceListEntryT*)self;
}

static void
iceListTerm(iceListHeadT* self) {
   self->prev = (iceListEntryT*)NULL;
   self->next = (iceListEntryT*)NULL;
}

static int
iceListIsEmpty(iceListHeadT* head) {
   return head->next == head;
}

static void
iceListAddInternal(iceListEntryT* _new, iceListEntryT* prev, iceListEntryT* next) {
   next->prev = _new;
   _new->next = next;
   _new->prev = prev;
   prev->next = _new;
}

static void
iceListPushFront(iceListHeadT* head, iceListEntryT* entry) {
   iceListAddInternal(entry, (iceListEntryT*)head, head->next);
}

static void
iceListPushBack(iceListHeadT* head, iceListEntryT* entry) {
   iceListAddInternal(entry, head->prev, (iceListEntryT*)head);
}

static iceListEntryT*
iceListGetFront(iceListHeadT* head) {
	iceListEntryT* entry = head->next;
	if (entry != head) {
		return entry;
	}

	return NULL;
}

static void
iceListRemoveInternal(iceListEntryT* prev, iceListEntryT* next) {
   next->prev = prev;
   prev->next = next;
}

static void
iceListRemove(iceListEntryT* entry) {
   iceListRemoveInternal(entry->prev, entry->next);
}
